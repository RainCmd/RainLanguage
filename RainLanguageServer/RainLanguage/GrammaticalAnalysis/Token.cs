using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal enum TokenType
    {
        Invalid,

        LogicOperationPriority = 0x10,
        LogicAnd,           // &&
        LogicOr,            // ||

        CompareOperationPriority = 0x20,
        Less,               // <
        Greater,            // >
        LessEquals,         // <=
        GreaterEquals,      // >=
        Equals,             // ==
        NotEquals,          // !=

        BitOperationPriority = 0x30,
        BitAnd,             // &
        BitOr,              // |
        BitXor,             // ^
        ShiftLeft,          // <<
        ShiftRight,         // >>

        ElementaryOperationPriority = 0x40,
        Plus,               // +
        Minus,              // -

        IntermediateOperationPriority = 0x50,
        Mul,                // *
        Div,                // /
        Mod,                // %

        SymbolicOperationPriority = 0x60,
        Casting,            // 类型转换
        Not,                // !
        Inverse,            // ~
        Positive,           // 正号( + )
        Negative,           // 负号( - )
        IncrementLeft,      // 左自增( ++X )
        DecrementLeft,		// 左自减( --X )
    }
    internal readonly struct Token(Lexical lexical, TokenType type)
    {
        public readonly Lexical lexical = lexical;
        public readonly TokenType type = type;
        public int Priority => (int)type >> 4;
        public ExpressionAttribute Precondition
        {
            get
            {
                switch (type)
                {
                    case TokenType.Invalid:
                    case TokenType.LogicOperationPriority: break;
                    case TokenType.LogicAnd:
                    case TokenType.LogicOr: return ExpressionAttribute.Value;
                    case TokenType.CompareOperationPriority: break;
                    case TokenType.Less:
                    case TokenType.Greater:
                    case TokenType.LessEquals:
                    case TokenType.GreaterEquals:
                    case TokenType.Equals:
                    case TokenType.NotEquals: return ExpressionAttribute.Value;
                    case TokenType.BitOperationPriority: break;
                    case TokenType.BitAnd:
                    case TokenType.BitOr:
                    case TokenType.BitXor:
                    case TokenType.ShiftLeft:
                    case TokenType.ShiftRight: return ExpressionAttribute.Value;
                    case TokenType.ElementaryOperationPriority: break;
                    case TokenType.Plus:
                    case TokenType.Minus: return ExpressionAttribute.Value;
                    case TokenType.IntermediateOperationPriority: break;
                    case TokenType.Mul:
                    case TokenType.Div:
                    case TokenType.Mod: return ExpressionAttribute.Value;
                    case TokenType.SymbolicOperationPriority: break;
                    case TokenType.Casting: return ExpressionAttribute.Type;
                    case TokenType.Not:
                    case TokenType.Inverse:
                    case TokenType.Positive:
                    case TokenType.Negative:
                    case TokenType.IncrementLeft:
                    case TokenType.DecrementLeft: return ExpressionAttribute.None | ExpressionAttribute.Operator;
                }
                return ExpressionAttribute.Invalid;
            }
        }
    }
}
