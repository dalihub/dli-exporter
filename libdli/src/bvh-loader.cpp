#include "libdli/bvh-loader.h"
#include "libdli/string-view.h"
#include "libdli/utils.h"
#include "dali/devel-api/common/map-wrapper.h"
#include <fstream>
#include <sstream>

#define ENUM_STRING_MAPPING(t, x) { #x, t::x }
#define TAG(x) const dli::StringView TAG_##x(#x)

using namespace Dali;

namespace dli
{
namespace
{

struct BvhChannel
{
  enum Type
  {
    Xposition,
    Yposition,
    Zposition,
    Xrotation,
    Yrotation,
    Zrotation,
    INVALID
  };

  enum
  {
    MASK_POSITION = NthBit(Xposition) | NthBit(Yposition) | NthBit(Zposition),
    MASK_ROTATION = NthBit(Xrotation) | NthBit(Yrotation) | NthBit(Zrotation)
  };

  struct Data
  {
    float mData[INVALID];

    Vector3 GetPosition() const
    {
      return Vector3(mData + Xposition);
    }

    Quaternion GetRotation() const
    {
      auto qz = Quaternion(Radian(Degree(mData[Zrotation])), Vector3::ZAXIS);
      auto qx = Quaternion(Radian(Degree(mData[Xrotation])), Vector3::XAXIS);
      qx = qz * qx;

      auto qy = Quaternion(Radian(Degree(mData[Yrotation])), Vector3::YAXIS);
      qx = qx * qy;

      return qx;
    }
  };

  static Type FromString(const char* str, size_t size);
};

const std::map<StringView, BvhChannel::Type> CHANNEL_STRINGS {
  ENUM_STRING_MAPPING(BvhChannel::Type, Xposition),
  ENUM_STRING_MAPPING(BvhChannel::Type, Yposition),
  ENUM_STRING_MAPPING(BvhChannel::Type, Zposition),
  ENUM_STRING_MAPPING(BvhChannel::Type, Xrotation),
  ENUM_STRING_MAPPING(BvhChannel::Type, Yrotation),
  ENUM_STRING_MAPPING(BvhChannel::Type, Zrotation),
};

BvhChannel::Type BvhChannel::FromString(const char* str, size_t size)
{
  auto iFind = CHANNEL_STRINGS.find(StringView(str, size));
  if (iFind != CHANNEL_STRINGS.end())
  {
    return iFind->second;
  }
  return Type::INVALID;
}

}

struct BvhHierarchy
{
  struct Node  // NOTE: we're ignoring the offset
  {
    std::string mName;
    Node* mParent;

    uint32_t mChannelMask = 0x0;
    std::vector<BvhChannel::Type> mChannels;

    Node(std::string&& name, Node* parent)
    :  mName(name),
      mParent(parent)
    {}
  };

  std::vector<Node*> mJoints;
  float mScale = 1.0f;

  BvhHierarchy() = default;
  BvhHierarchy(BvhHierarchy&& other)
  :  mJoints(std::move(other.mJoints)),
    mScale(other.mScale)
  {}

  ~BvhHierarchy()
  {
    for (auto j : mJoints)
    {
      delete j;
    }
  }
};

namespace
{

TAG(HIERARCHY);
TAG(ROOT);
TAG(OFFSET);
TAG(CHANNELS);
TAG(JOINT);

const StringView TAG_END("End");
const StringView TAG_SITE("Site");

struct Context
{
  std::ifstream file;
  std::istringstream tokens;

  BvhHierarchy::Node* last;
  BvhHierarchy hierarchy;

  void ReadNextLine(std::string& line)
  {
    if (!std::getline(file, line))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to read next line.";
    }
  }

  void SetTokens(const std::string& str)
  {
    tokens.clear();
    tokens.str(str);
  }

  void ReadNextLineAndSetTokens(std::string& line)
  {
    ReadNextLine(line);
    SetTokens(line);
  }

  void Expect(StringView const& sv, std::string& data)
  {
    if (!((tokens >> data) && sv.Compare(data) == 0))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Expected '" << sv.ToString() << "', got '" << data << "'.";
    }
  }
};

void IgnoreJointOffset(Context& ctx)
{
  float dummy;
  for (uint8_t i = 0; i < 3; ++i)
  {
    if (!(ctx.tokens >> dummy))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to parse offset.";
    }
  }
}

void ReadJoint(Context& ctx)
{
  std::string data;
  ctx.ReadNextLineAndSetTokens(data);
  ctx.Expect("{", data);

  ctx.ReadNextLineAndSetTokens(data);
  ctx.Expect(TAG_OFFSET, data);
  IgnoreJointOffset(ctx);

  ctx.ReadNextLineAndSetTokens(data);
  ctx.Expect(TAG_CHANNELS, data);

  uint32_t numChannels;
  if (!((ctx.tokens >> numChannels) && numChannels > 0))
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Expected number of channels, got: '" << data << "'.";
  }

  ctx.last->mChannels.resize(numChannels, BvhChannel::INVALID);
  for (auto& c : ctx.last->mChannels)
  {
    if (!(ctx.tokens >> data))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Expected: channel name, got: '" << data << "'.";
    }

    c = BvhChannel::FromString(data.c_str(), data.size());
    if (c == BvhChannel::INVALID)
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Not a valid channel: " + data;
    }

    ctx.last->mChannelMask |= NthBit(c);
  }

  // read child joints
  ctx.ReadNextLineAndSetTokens(data);
  if (!(ctx.tokens >> data))
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Failed to read next token.";
  }

  if (TAG_END.Compare(data) == 0)
  {
    ctx.Expect(TAG_SITE, data);  // ignore Site

    ctx.ReadNextLineAndSetTokens(data);
    ctx.Expect("{", data);

    ctx.ReadNextLineAndSetTokens(data);
    ctx.Expect(TAG_OFFSET, data);
    IgnoreJointOffset(ctx);

    ctx.ReadNextLineAndSetTokens(data);
    ctx.Expect("}", data);

    ctx.ReadNextLineAndSetTokens(data);
    ctx.Expect("}", data);
  }
  else
  {
    while (TAG_JOINT.Compare(data) == 0)
    {
      if (!(ctx.tokens >> data))
      {
        ExceptionFlinger(ASSERT_LOCATION) << "Expected joint name, got '" << data << "'.";
      }

      ctx.hierarchy.mJoints.push_back(new BvhHierarchy::Node{ std::move(data), ctx.last });
      ctx.last = ctx.hierarchy.mJoints.back();

      ReadJoint(ctx);

      ctx.ReadNextLineAndSetTokens(data);
      if (!(ctx.tokens >> data))
      {
        ExceptionFlinger(ASSERT_LOCATION) << "Failed to read next token.";
      }
    }

    if (!(TAG_JOINT.Compare(data) == 0 || data.compare("}") == 0))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Invalid token: " << data;
    }
  }

  ctx.last = ctx.last->mParent;
}

void ReadHierarchy(Context& ctx)
{
  std::string data;
  ctx.ReadNextLineAndSetTokens(data);
  ctx.Expect(TAG_ROOT, data);

  if (!(ctx.tokens >> data))
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Expected root joint name, got '" << data << "'.";
  }

  ctx.hierarchy.mJoints.push_back(new BvhHierarchy::Node{ std::move(data), nullptr });
  ctx.last = ctx.hierarchy.mJoints.back();

  ReadJoint(ctx);
}

void FindHierarchy(Context& ctx)
{
  std::string line;
  ctx.ReadNextLine(line);

  if (TAG_HIERARCHY.Compare(line) != 0)
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Expected '" + TAG_HIERARCHY.ToString() + "', got '" + line + "'.";
  }
}

struct MotionHeader
{
  uint32_t numFrames;
  float frameDelaySeconds;
  uint32_t numChannels;  // must match hierarchy
};

void Dispose(BvhHierarchy* hierarchy)
{
  delete hierarchy;
}

}

BhvHierarchyPtr LoadBvhHierarchy(const std::string & url, float scale)
{
  Context ctx{ std::ifstream(url), std::istringstream{}, nullptr };
  if (!ctx.file.is_open())
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Failed to open '" << url << "'.";
  }

  FindHierarchy(ctx);
  ReadHierarchy(ctx);

  ctx.hierarchy.mScale = scale;
  return BhvHierarchyPtr(new BvhHierarchy(std::move(ctx.hierarchy)), Dispose);
}

AnimationDefinition LoadBvhMotion(const std::string& url, BvhHierarchy const& hierarchy)
{
  std::ifstream file(url, std::ios::binary);
  AnimationDefinition animDef;
  if (file)
  {
    MotionHeader header;
    if (!file.read(reinterpret_cast<char*>(&header), sizeof(header)))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Failed to read the header.";
    }

    auto iSlash = url.rfind('/') + 1;
    animDef.mName = url.substr(iSlash, iSlash != std::string::npos ?
      url.find('.', iSlash) - iSlash : std::string::npos);
    animDef.mDuration = header.frameDelaySeconds * header.numFrames;
    float progressPerFrame = 1.f / header.numFrames;

    auto iChannel = file.tellg();
    for (auto j : hierarchy.mJoints)
    {
      KeyFrames kfPosition;
      if (j->mChannelMask & BvhChannel::MASK_POSITION)
      {
        kfPosition = KeyFrames::New();
      }

      KeyFrames kfRotation;
      if (j->mChannelMask & BvhChannel::MASK_ROTATION)
      {
        kfRotation = KeyFrames::New();
      }

      float progress = 0.f;
      for (uint32_t i = 0; i < header.numFrames; ++i)
      {
        BvhChannel::Data channel{};
        for (auto c : j->mChannels)
        {
          if (!file.read(reinterpret_cast<char*>(channel.mData + c), sizeof(float)))
          {
            ExceptionFlinger(ASSERT_LOCATION) << "Failed to read motion data.";
          }
        }

        if (kfPosition)
        {
          auto position = channel.GetPosition() * hierarchy.mScale;
          kfPosition.Add(progress, position);
        }

        if (kfRotation)
        {
          auto rotation = channel.GetRotation();
          kfRotation.Add(progress, rotation);
        }

        auto skip = (header.numChannels - j->mChannels.size()) * sizeof(float);
        if (!file.seekg(skip, std::ios::cur))
        {
          ExceptionFlinger(ASSERT_LOCATION) << "Failed to read motion data.";
        }
        progress += progressPerFrame;
      }

      if (kfPosition)
      {
        AnimatedProperty animProp;
        animProp.mNodeName = j->mName;
        animProp.mPropertyName = "position";
        animProp.mKeyFrames = kfPosition;
        animProp.mTimePeriod.durationSeconds = animDef.mDuration;
        animDef.mProperties.push_back(std::move(animProp));
      }

      if (kfRotation)
      {
        AnimatedProperty animProp;
        animProp.mNodeName = j->mName;
        animProp.mPropertyName = "orientation";
        animProp.mKeyFrames = kfRotation;
        animProp.mTimePeriod.durationSeconds = animDef.mDuration;
        animDef.mProperties.push_back(std::move(animProp));
      }

      iChannel += j->mChannels.size() * sizeof(float);
      if (!file.seekg(iChannel, std::ios::beg))
      {
        ExceptionFlinger(ASSERT_LOCATION) << "Failed to read motion data.";
      }
    }
  }

  return animDef;
}

}
