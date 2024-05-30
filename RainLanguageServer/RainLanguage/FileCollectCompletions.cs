using LanguageServer.Parameters;
using System.Diagnostics.CodeAnalysis;

namespace RainLanguageServer.RainLanguage
{
    internal static class FileCollectCompletions
    {
        private static void CollectTypes(CompilingSpace space, HashSet<Declaration> declarations, Predicate<Declaration>? condition)
        {
            foreach (var list in space.declarations.Values)
                if (list != null)
                    foreach (var item in list)
                    {
                        var declaration = item.declaration;
                        if (condition == null || condition(declaration))
                            switch (declaration.category)
                            {
                                case DeclarationCategory.Enum:
                                case DeclarationCategory.Struct:
                                case DeclarationCategory.Class:
                                case DeclarationCategory.Interface:
                                case DeclarationCategory.Delegate:
                                case DeclarationCategory.Task:
                                    declarations.Add(declaration);
                                    break;
                            }
                    }
        }
        public static void CollectTypes(ASTManager manager, FileSpace space, List<CompletionInfo> infos, Predicate<Declaration>? condition)
        {
            var set = new HashSet<Declaration>();
            for (var index = space; index != null; index = index.parent)
            {
                foreach (var file in index.enums)
                    if (file.compiling != null)
                        if (condition == null || condition(file.compiling.declaration))
                            set.Add(file.compiling.declaration);
                foreach (var file in index.structs)
                    if (file.compiling != null)
                        if (condition == null || condition(file.compiling.declaration))
                            set.Add(file.compiling.declaration);
                foreach (var file in index.interfaces)
                    if (file.compiling != null)
                        if (condition == null || condition(file.compiling.declaration))
                            set.Add(file.compiling.declaration);
                foreach (var file in index.classes)
                    if (file.compiling != null)
                        if (condition == null || condition(file.compiling.declaration))
                            set.Add(file.compiling.declaration);
                foreach (var file in index.delegates)
                    if (file.compiling != null)
                        if (condition == null || condition(file.compiling.declaration))
                            set.Add(file.compiling.declaration);
                foreach (var file in index.tasks)
                    if (file.compiling != null)
                        if (condition == null || condition(file.compiling.declaration))
                            set.Add(file.compiling.declaration);
            }
            foreach (var rely in space.relies) CollectTypes(rely, set, condition);
            foreach (var rely in manager.relies.Values) CollectTypes(rely, set, condition);
            foreach (var declaration in set)
            {
                if (declaration.category == DeclarationCategory.Enum || declaration.category == DeclarationCategory.Struct)
                    infos.Add(new CompletionInfo(declaration.name[^1], LanguageServer.Parameters.TextDocument.CompletionItemKind.Struct, declaration.ToString()));
                else
                    infos.Add(new CompletionInfo(declaration.name[^1], LanguageServer.Parameters.TextDocument.CompletionItemKind.Class, declaration.ToString()));
            }
        }
        private static bool TryFindSpace(ASTManager manager, FileSpace file, string name, [MaybeNullWhen(false)] out CompilingSpace space)
        {
            for (var index = file.compiling; index != null; index = index.parent)
                if (index.children.TryGetValue(name, out space)) return true;

            space = manager.LoadLibrary(name);
            return space != null;
        }

        private static bool ParseTypeList(ASTManager manager, FileSpace space, TextRange range, ref TextPosition index, List<CompletionInfo> infos)
        {
            while (Lexical.TryExtractName(range, index, out var end, out _, null))
            {
                index = end;
                Lexical.ExtractDimension(range, ref index);
                if (Lexical.TryAnalysis(range, index, out var lexical, null))
                {
                    if (lexical.type == LexicalType.Comma || lexical.type == LexicalType.Semicolon)
                    {
                        index = lexical.anchor.end;
                        continue;
                    }
                    break;
                }
                break;
            }
            if (Lexical.TryAnalysis(range, index, out _, null)) return false;
            CollectTypes(manager, space, infos, null);
            return true;
        }
        private static bool ParseParameterList(ASTManager manager, FileSpace space, TextRange range, ref TextPosition index, List<CompletionInfo> infos)
        {
            if (!Lexical.TryAnalysis(range, index, out var lexical, null) || lexical.type != LexicalType.BracketLeft0) return false;
            index = lexical.anchor.end;
            while (Lexical.TryExtractName(range, index, out var end, out _, null))
            {
                index = end;
                Lexical.ExtractDimension(range, ref index);
                if (Lexical.TryAnalysis(range, index, out lexical, null))
                {
                    index = lexical.anchor.end;
                    if (lexical.type == LexicalType.Comma || lexical.type == LexicalType.Semicolon) continue;
                    else if (lexical.type == LexicalType.Word)
                        if (Lexical.TryAnalysis(range, index, out lexical, null) && (lexical.type == LexicalType.Comma || lexical.type == LexicalType.Semicolon))
                            continue;
                    return false;
                }
                break;
            }
            if (Lexical.TryAnalysis(range, index, out _, null)) return false;
            CollectTypes(manager, space, infos, null);
            return true;
        }
        public static bool CollectDefines(ASTManager manager, FileSpace space, TextRange range, TextPosition index, List<CompletionInfo> infos)
        {
            if (ParseTypeList(manager, space, range, ref index, infos)) return true;
            if (Lexical.TryAnalysis(range, index, out var lexical, null) && lexical.type == LexicalType.Word)
            {
                index = lexical.anchor.end;
                return ParseParameterList(manager, space, range, ref index, infos);
            }
            return false;
        }
        public static bool CollectDefines(ASTManager manager, FileSpace space, bool visibility, TextRange range, TextPosition index, List<CompletionInfo> infos)
        {
            if (Lexical.TryAnalysis(range, index, out var lexical, null))
            {
                switch (lexical.type)
                {
                    case LexicalType.Word:
                        return CollectDefines(manager, space, range, index, infos);
                    case LexicalType.KeyWord_namespace:
                        {
                            if (Lexical.TryExtractName(range, lexical.anchor.end, out var idx, out var names, null))
                            {
                                if (Lexical.TryAnalysis(range, idx, out lexical, null))
                                {
                                    if (lexical.type == LexicalType.Dot && !Lexical.TryAnalysis(range, lexical.anchor.end, out _, null))
                                    {
                                        var compiling = space.compiling;
                                        foreach (var name in names)
                                            if (!compiling.children.TryGetValue(name.ToString(), out compiling))
                                                return false;
                                        foreach (var child in compiling.children)
                                            infos.Add(new CompletionInfo(child.Key, LanguageServer.Parameters.TextDocument.CompletionItemKind.Module, "命名空间"));
                                    }
                                }
                                else
                                {
                                    var compiling = space.compiling;
                                    for (var i = 0; i < names.Count - 1; i++)
                                        if (!compiling.children.TryGetValue(names[i].ToString(), out compiling))
                                            return false;
                                    foreach (var child in compiling.children)
                                        infos.Add(new CompletionInfo(child.Key, LanguageServer.Parameters.TextDocument.CompletionItemKind.Module, "命名空间"));
                                }
                            }
                            else foreach (var child in space.compiling.children)
                                    infos.Add(new CompletionInfo(child.Key, LanguageServer.Parameters.TextDocument.CompletionItemKind.Module, "命名空间"));
                            return true;
                        }
                    case LexicalType.KeyWord_import:
                        {
                            if (Lexical.TryExtractName(range, lexical.anchor.end, out var idx, out var names, null))
                            {
                                if (Lexical.TryAnalysis(range, idx, out lexical, null))
                                {
                                    if (lexical.type == LexicalType.Dot && !Lexical.TryAnalysis(range, lexical.anchor.end, out _, null))
                                    {
                                        if (!TryFindSpace(manager, space, names[0].ToString(), out var compilingSpace)) return false;
                                        for (var i = 1; i < names.Count; i++)
                                            if (!compilingSpace.children.TryGetValue(names[i].ToString(), out compilingSpace))
                                                return false;
                                        foreach (var child in compilingSpace.children)
                                            infos.Add(new CompletionInfo(child.Key, LanguageServer.Parameters.TextDocument.CompletionItemKind.Module, "命名空间"));
                                    }
                                }
                                else
                                {
                                    for (var spaceIndex = space.compiling; spaceIndex != null; spaceIndex = spaceIndex.parent)
                                        foreach (var child in spaceIndex.children.Keys)
                                            if (child != spaceIndex.name)
                                                infos.Add(new CompletionInfo(child, LanguageServer.Parameters.TextDocument.CompletionItemKind.Module, "命名空间"));
                                    foreach (var rely in manager.relies.Values)
                                        infos.Add(new CompletionInfo(rely.name, LanguageServer.Parameters.TextDocument.CompletionItemKind.Module, "库"));
                                }
                                return true;
                            }
                        }
                        return true;
                    case LexicalType.KeyWord_native:
                        return CollectDefines(manager, space, range, lexical.anchor.end, infos);
                    case LexicalType.KeyWord_public:
                    case LexicalType.KeyWord_internal:
                    case LexicalType.KeyWord_space:
                    case LexicalType.KeyWord_protected:
                    case LexicalType.KeyWord_private:
                        return CollectDefines(manager, space, true, range, lexical.anchor.end, infos);
                    case LexicalType.KeyWord_class:
                        if (Lexical.TryAnalysis(range, lexical.anchor.end, out lexical, null) && lexical.type == LexicalType.Word)
                        {
                            if (Lexical.TryAnalysis(range, lexical.anchor.end, out lexical, null))
                            {
                                if (Lexical.TryAnalysis(range, lexical.anchor.end, out _, null))
                                    CollectTypes(manager, space, infos, value => value.category == DeclarationCategory.Interface);
                                else
                                    CollectTypes(manager, space, infos, value => value.category == DeclarationCategory.Interface || value.category == DeclarationCategory.Class);
                                return true;
                            }
                        }
                        return false;
                    case LexicalType.KeyWord_interface:
                        if (Lexical.TryAnalysis(range, lexical.anchor.end, out lexical, null) && lexical.type == LexicalType.Word)
                        {
                            CollectTypes(manager, space, infos, value => value.category == DeclarationCategory.Interface);
                            return true;
                        }
                        return false;
                    case LexicalType.KeyWord_const:
                        return CollectDefines(manager, space, range, lexical.anchor.end, infos);
                    case LexicalType.KeyWord_bool:
                    case LexicalType.KeyWord_byte:
                    case LexicalType.KeyWord_char:
                    case LexicalType.KeyWord_integer:
                    case LexicalType.KeyWord_real:
                    case LexicalType.KeyWord_real2:
                    case LexicalType.KeyWord_real3:
                    case LexicalType.KeyWord_real4:
                    case LexicalType.KeyWord_type:
                    case LexicalType.KeyWord_string:
                    case LexicalType.KeyWord_handle:
                    case LexicalType.KeyWord_entity:
                        return CollectDefines(manager, space, range, range.start, infos);
                    case LexicalType.KeyWord_delegate:
                    case LexicalType.KeyWord_task:
                        return CollectDefines(manager, space, range, lexical.anchor.end, infos);
                }
            }
            else
            {
                if (!visibility)
                    CompletionInfo.CollectAccessKeyword(infos);
                CompletionInfo.CollectStructKeyword(infos);
                CompletionInfo.CollectDefineKeyword(infos);
                return true;
            }
            return false;
        }
        public static bool CollectCompletions(ASTManager manager, FileSpace space, TextPosition position, List<CompletionInfo> infos)
        {
            var line = position.Line;
            if (line.indent < 0) return false;
            foreach (var child in space.children)
                if (child.range.Contain(position))
                    return CollectCompletions(manager, space, position, infos);
            foreach (var declaration in space.SelfDeclarations)
                if (declaration.range.Contain(position))
                    return declaration.CollectCompletions(manager, position, infos);

            return CollectDefines(manager, space, false, line.start & position, line.start, infos);
        }
    }
}
