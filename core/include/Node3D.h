#ifndef NODE3D_H
#define NODE3D_H
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "Vector3.h"
#include "Vector2.h"
#include "Matrix.h"

#include "assimp/scene.h"

#include <vector>
#include <functional>
#include <memory>

using namespace std;

class Node3D
{
public:
    enum { INVALID_MESH = -1 };

    using Predicate = std::function<bool(const Node3D&)>;
    static const Predicate DEFAULT_END_VISIT_PREDICATE;

    static std::string MakeValidName(const std::string & name);

    Node3D(Node3D *eParent);
    virtual ~Node3D();
    bool HasMesh() const;

    template <class Visitor>
    inline
    void Visit(Visitor v, Predicate endVisitPredicate = DEFAULT_END_VISIT_PREDICATE)
    {
        VisitInternal(*this, v, endVisitPredicate);
    }

    template <class Visitor>
    inline
    void Visit(Visitor v, Predicate endVisitPredicate = DEFAULT_END_VISIT_PREDICATE) const
    {
        VisitInternal(*this, v, endVisitPredicate);
    }

    bool IsSkeletonRoot() const;

    ///@brief Generate the list of joints, which the mesh information indexes
    /// into. These will be in a depth first order.
    ///@note This is not really meaningful for a node that isn't a skeletal
    /// root, though that isn't enforced.
    std::vector<const Node3D*> GetJoints() const;

    unsigned int m_Index;
    string m_Name;
    Node3D *m_Parent;
    Matrix m_Matrix;
    int m_MaterialIdx;
    bool m_isBlendEnabled;

    vector<Node3D*> m_Children;
    unsigned int m_MeshId = INVALID_MESH;

    Node3D* m_Skeleton; // no ownership; shows which skeleton this node is a part of (if not nullptr).
    std::unique_ptr<Matrix> m_InverseBindPoseMatrix;    // only joint nodes have this.

private:
    template <class Node3D_, class Visitor>
    static void VisitInternal(Node3D_& n, Visitor v, Predicate endVisitPredicate)
    {
        v(n);
        if (!endVisitPredicate(n))
        {
            for (auto& i : n.m_Children)
            {
                i->Visit(v);
            }
        }
    }
};

#endif // NODE3D_H
