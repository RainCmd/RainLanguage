﻿#pragma once
#include "Attribute.h"
#include "../Anchor.h"
struct ExpressionParser;
class Expression;

Expression* CreateLessOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateGreaterOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateLessEqualsOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateGreaterEqualsOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateEqualsOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateNotEqualsOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateBitAndOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateBitOrOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateBitXorOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateShiftLeftOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateShiftRightOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreatePlusOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateMinusOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateMulOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateDivOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateModOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);

Expression* CreateNotOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateInverseOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreatePositiveOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateNegativeOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateIncrementLeftOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateDecrementLeftOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateIncrementRightOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateDecrementRightOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);