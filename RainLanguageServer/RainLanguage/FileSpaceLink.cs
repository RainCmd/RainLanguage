using System.Text;

namespace RainLanguageServer.RainLanguage
{
    internal partial class FileSpace
    {
        private Type GetType(Context context, ASTManager manager, FileType fileType)
        {
            Type result = default;
            if (context.TryFindDeclaration(manager, fileType.name, out var declarations, collector))
            {
                if (declarations.Count == 1)
                {
                    result = declarations[0].Declaration.GetDefineType();
                    if (!result.Vaild) collector.Add(fileType.name, CErrorLevel.Error, "无效的类型");
                }
                else
                {
                    var builder = new StringBuilder().AppendLine("类型不明确");
                    foreach (var item in declarations)
                        builder.AppendLine(item.GetFullName());
                    collector.Add(fileType.name, CErrorLevel.Error, builder.ToString());
                }
            }
            else collector.Add(fileType.name, CErrorLevel.Error, "声明未找到");
            return result;
        }
        private void AddTypeCite(ASTManager manager, CompilingDeclaration source, Type type)
        {
            if (manager.GetSourceDeclaration(type) is CompilingDeclaration target)
                source.AddCite(target.file);
        }
        private void AddTypeCites(ASTManager manager, CompilingDeclaration source, IList<Type> types)
        {
            foreach (var type in types) AddTypeCite(manager, source, type);
        }
        public void Link(ASTManager manager)
        {
            foreach (var child in children) child.Link(manager);
            var context = new Context(compiling, relies, default);
            foreach (var file in variables)
            {
                Type type = GetType(context, manager, file.type);
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Variable, compiling.GetChildName(file.name.ToString()), default);
                var variable = new CompilingVariable(file.name, declaration, compiling, file, file.isReadonly, type, file.expression, relies);
                manager.library.variables.Add(variable);
                compiling.AddDeclaration(variable);
                AddTypeCite(manager,variable, type);
            }
            foreach (var file in functions)
            {
                var parameters = new List<CompilingCallable.Parameter>();
                foreach (var parameter in file.parameters)
                    parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                var parameterTypes = new List<Type>();
                foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                var returnTypes = new List<Type>();
                foreach (var type in file.returns)
                    returnTypes.Add(GetType(context, manager, type));
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Function, compiling.GetChildName(file.name.ToString()), new Tuple(parameterTypes));
                var function = new CompilingFunction(file.name, declaration, compiling, file, parameters, new Tuple(returnTypes), file.body, relies);
                manager.library.functions.Add(function);
                compiling.AddDeclaration(function);
                AddTypeCites(manager, function, parameterTypes);
                AddTypeCites(manager, function, returnTypes);
            }
            foreach(var file in enums)
            {
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Enum, compiling.GetChildName(file.name.ToString()), default);
                var compilingEnum = new CompilingEnum(file.name, declaration, compiling, file);

            }
        }
    }
}
