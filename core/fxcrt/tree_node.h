// Copyright 2019 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FXCRT_TREE_NODE_H_
#define CORE_FXCRT_TREE_NODE_H_

#include <stdint.h>

#include "core/fxcrt/check.h"
#include "core/fxcrt/unowned_ptr_exclusion.h"

namespace fxcrt {

// Implements the usual DOM/XML-ish trees allowing for a variety of
// pointer types with which to connect the nodes. Public methods maintain
// the invariants of the tree.

template <typename T>
class TreeNodeBase {
 public:
  TreeNodeBase() = default;
  virtual ~TreeNodeBase() = default;

  inline T* GetParent() const { return static_cast<const T*>(this)->parent_; }
  inline T* GetFirstChild() const {
    return static_cast<const T*>(this)->first_child_;
  }
  inline T* GetLastChild() const {
    return static_cast<const T*>(this)->last_child_;
  }
  inline T* GetNextSibling() const {
    return static_cast<const T*>(this)->next_sibling_;
  }
  inline T* GetPrevSibling() const {
    return static_cast<const T*>(this)->prev_sibling_;
  }

  bool HasChild(const T* child) const {
    return child != this && child->GetParent() == this;
  }

  T* GetNthChild(int32_t n) {
    if (n < 0) {
      return nullptr;
    }
    T* result = GetFirstChild();
    while (n-- && result) {
      result = result->GetNextSibling();
    }
    return result;
  }

  void AppendFirstChild(T* child) {
    BecomeParent(child);
    if (GetFirstChild()) {
      CHECK(GetLastChild());
      GetFirstChild()->SetPrevSibling(child);
      child->SetNextSibling(GetFirstChild());
      SetFirstChild(child);
    } else {
      CHECK(!GetLastChild());
      SetFirstChild(child);
      SetLastChild(child);
    }
  }

  void AppendLastChild(T* child) {
    BecomeParent(child);
    if (GetLastChild()) {
      CHECK(GetFirstChild());
      GetLastChild()->SetNextSibling(child);
      child->SetPrevSibling(GetLastChild());
      SetLastChild(child);
    } else {
      CHECK(!GetFirstChild());
      SetFirstChild(child);
      SetLastChild(child);
    }
  }

  void InsertBefore(T* child, T* other) {
    if (!other) {
      AppendLastChild(child);
      return;
    }
    BecomeParent(child);
    CHECK(HasChild(other));
    child->SetNextSibling(other);
    child->SetPrevSibling(other->GetPrevSibling());
    if (GetFirstChild() == other) {
      CHECK(!other->GetPrevSibling());
      SetFirstChild(child);
    } else {
      other->GetPrevSibling()->SetNextSibling(child);
    }
    other->prev_sibling_ = child;
  }

  void InsertAfter(T* child, T* other) {
    if (!other) {
      AppendFirstChild(child);
      return;
    }
    BecomeParent(child);
    CHECK(HasChild(other));
    child->SetNextSibling(other->GetNextSibling());
    child->SetPrevSibling(other);
    if (GetLastChild() == other) {
      CHECK(!other->GetNextSibling());
      SetLastChild(child);
    } else {
      other->GetNextSibling()->SetPrevSibling(child);
    }
    other->SetNextSibling(child);
  }

  void RemoveChild(T* child) {
    CHECK(HasChild(child));
    if (GetLastChild() == child) {
      CHECK(!child->GetNextSibling());
      SetLastChild(child->GetPrevSibling());
    } else {
      child->GetNextSibling()->SetPrevSibling(child->GetPrevSibling());
    }
    if (GetFirstChild() == child) {
      CHECK(!child->GetPrevSibling());
      SetFirstChild(child->GetNextSibling());
    } else {
      child->GetPrevSibling()->SetNextSibling(child->GetNextSibling());
    }
    child->SetParent(nullptr);
    child->SetPrevSibling(nullptr);
    child->SetNextSibling(nullptr);
  }

  void RemoveAllChildren() {
    while (T* child = GetFirstChild()) {
      RemoveChild(child);
    }
  }

  void RemoveSelfIfParented() {
    if (T* parent = GetParent()) {
      parent->RemoveChild(static_cast<T*>(this));
    }
  }

 private:
  // These are private because they may leave the tree in an invalid state
  // until subsequent operations restore it.
  inline void SetParent(T* pParent) {
    static_cast<T*>(this)->parent_ = pParent;
  }
  inline void SetFirstChild(T* pChild) {
    static_cast<T*>(this)->first_child_ = pChild;
  }
  inline void SetLastChild(T* pChild) {
    static_cast<T*>(this)->last_child_ = pChild;
  }
  inline void SetNextSibling(T* pSibling) {
    static_cast<T*>(this)->next_sibling_ = pSibling;
  }
  inline void SetPrevSibling(T* pSibling) {
    static_cast<T*>(this)->prev_sibling_ = pSibling;
  }

  // Child left in state where sibling members need subsequent adjustment.
  void BecomeParent(T* child) {
    CHECK(child != this);  // Detect attempts at self-insertion.
    if (child->parent_) {
      child->parent_->TreeNodeBase<T>::RemoveChild(child);
    }
    child->parent_ = static_cast<T*>(this);
    CHECK(!child->next_sibling_);
    CHECK(!child->prev_sibling_);
  }
};

// Tree connected using C-style pointers.
template <typename T>
class TreeNode : public TreeNodeBase<T> {
 public:
  TreeNode() = default;
  virtual ~TreeNode() = default;

 private:
  friend class TreeNodeBase<T>;

  UNOWNED_PTR_EXCLUSION T* parent_ = nullptr;        // intra-tree pointer.
  UNOWNED_PTR_EXCLUSION T* first_child_ = nullptr;   // intra-tree pointer.
  UNOWNED_PTR_EXCLUSION T* last_child_ = nullptr;    // intra-tree pointer.
  UNOWNED_PTR_EXCLUSION T* next_sibling_ = nullptr;  // intra-tree pointer.
  UNOWNED_PTR_EXCLUSION T* prev_sibling_ = nullptr;  // intra-tree pointer.
};

}  // namespace fxcrt

using fxcrt::TreeNode;

#endif  // CORE_FXCRT_TREE_NODE_H_
