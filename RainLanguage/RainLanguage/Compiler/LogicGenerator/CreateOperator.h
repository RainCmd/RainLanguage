#pragma once
#include "Attribute.h"
#include "../Anchor.h"
struct ExpressionParser;
class Expression;

Expression* CreateLessOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateGreaterOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateLessEqualsOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateGreaterEqualsOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateEqualsOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateNotEqualsOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateBitAndOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateBitOrOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateBitXorOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateShiftLeftOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateShiftRightOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreatePlusOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateMinusOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateMulOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateDivOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);
Expression* CreateModOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters);

Expression* CreateNotOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateInverseOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreatePositiveOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateNegativeOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateIncrementLeftOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateDecrementLeftOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateIncrementRightOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);
Expression* CreateDecrementRightOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter);