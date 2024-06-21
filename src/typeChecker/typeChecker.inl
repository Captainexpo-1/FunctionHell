#ifndef TYPECHECKER_INL
#define TYPECHECKER_INL

#include "TypeChecker.hpp"

template <typename T>
bool TypeChecker::m_isType(ASTNode* node) {
    return dynamic_cast<T*>(node) != nullptr;
}

#endif // TYPECHECKER_INL