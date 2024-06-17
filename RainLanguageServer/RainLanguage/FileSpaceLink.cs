using RainLanguageServer.RainLanguage.GrammaticalAnalysis;

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
                    result = declarations[0].declaration.GetDefineType();
                    if (!result.Vaild) collector.Add(fileType.name, CErrorLevel.Error, "无效的类型");
                }
                else
                {
                    var msg = new CompileMessage(fileType.name, CErrorLevel.Error, "类型不明确");
                    foreach (var item in declarations)
                        msg.related.Add(new RelatedInfo(item.name, item.GetFullName()));
                    collector.Add(msg);
                }
            }
            else collector.Add(fileType.name, CErrorLevel.Error, "声明未找到");
            if (manager.GetSourceDeclaration(result) is CompilingDeclaration target)
                target.references.Add(fileType.GetNameRange());
            return result.GetDimensionType(fileType.dimension);
        }
        public void Link(ASTManager manager, CompilingLibrary library, bool cite)
        {
            foreach (var child in children) child.Link(manager, library, cite);
            var context = new Context(document, compiling, relies, default);
            foreach (var file in variables)
            {
                var type = GetType(context, manager, file.type);
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Variable, compiling.GetChildName(file.name.ToString()), default);
                var variable = new CompilingVariable(file.name, declaration, file.attributes, compiling, cite ? file : null, file.isReadonly, type, file.expression, relies);
                if (file.isReadonly && type != Type.BOOL && type != Type.BYTE && type != Type.CHAR && type != Type.INT && type != Type.REAL && type != Type.STRING)
                    file.space.collector.Add(file.name, CErrorLevel.Error, "该类型不能作为常量");
                library.variables.Add(variable);
                file.compiling = variable;
                compiling.AddDeclaration(variable);
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
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Function, compiling.GetChildName(file.name.ToString()), new Tuple(parameterTypes));
                var function = new CompilingFunction(file.name, declaration, file.attributes, compiling, cite ? file : null, parameters, new Tuple(returnTypes), new LogicBlock(document, null, parameters, returnTypes, compiling, file.body, relies, collector));
                library.functions.Add(function);
                file.compiling = function;
                compiling.AddDeclaration(function);
            }
            foreach (var file in enums)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingEnum = (CompilingEnum)declarations!.Find(value => value.name == file.name)!;

                foreach (var element in file.elements)
                {
                    var declaration = new Declaration(library.name, Visibility.Public, DeclarationCategory.EnumElement, compiling.GetMemberName(file.name.ToString(), element.name.ToString()), default);
                    var compilingEnumElement = new CompilingEnum.Element(element.name, declaration, cite ? element : null);
                    compilingEnum.elements.Add(compilingEnumElement);
                }
            }
            foreach (var file in structs)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingStruct = (CompilingStruct)declarations!.Find(value => value.name == file.name)!;

                foreach (var variable in file.variables)
                {
                    var type = GetType(context, manager, variable.type);
                    var declaration = new Declaration(library.name, variable.visibility, DeclarationCategory.StructVariable, compiling.GetMemberName(file.name.ToString(), variable.name.ToString()), default);
                    var compilingVariable = new CompilingVariable(variable.name, declaration, variable.attributes, compiling, cite ? variable : null, variable.isReadonly, type, variable.expression, relies);
                    compilingStruct.variables.Add(compilingVariable);
                    variable.compiling = compilingVariable;
                }
                foreach (var function in file.functions)
                {
                    var parameters = new List<CompilingCallable.Parameter>();
                    foreach (var parameter in function.parameters)
                        parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                    var parameterTypes = new List<Type>();
                    foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                    var returnTypes = new List<Type>();
                    foreach (var type in function.returns)
                        returnTypes.Add(GetType(context, manager, type));
                    var declaration = new Declaration(library.name, function.visibility, DeclarationCategory.StructFunction, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingFunction(function.name, declaration, function.attributes, compiling, cite ? function : null, parameters, new Tuple(returnTypes), new LogicBlock(document, compilingStruct, parameters, returnTypes, compiling, function.body, relies, collector));
                    compilingStruct.functions.Add(compilingFunction);
                    function.compiling = compilingFunction;
                }
            }
            foreach (var file in interfaces)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingInterface = (CompilingInterface)declarations!.Find(value => value.name == file.name)!;

                foreach (var fileType in file.inherits)
                {
                    var type = GetType(context, manager, fileType);
                    if (type.dimension > 0) collector.Add(fileType.name, CErrorLevel.Error, "不能继承数组");
                    else if (type.code != TypeCode.Interface && type != Type.HANDLE) collector.Add(fileType.name, CErrorLevel.Error, "必须是接口");
                    compilingInterface.inherits.Add(type);
                    if (manager.GetSourceDeclaration(type) is CompilingInterface inherit)
                        compilingInterface.implements.Add(inherit);
                }
                foreach (var function in file.functions)
                {
                    var parameters = new List<CompilingCallable.Parameter>();
                    foreach (var parameter in function.parameters)
                        parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                    var parameterTypes = new List<Type>();
                    foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                    var returnTypes = new List<Type>();
                    foreach (var type in function.returns)
                        returnTypes.Add(GetType(context, manager, type));
                    var declaration = new Declaration(library.name, function.visibility, DeclarationCategory.InterfaceFunction, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingAbstractFunction(function.name, declaration, function.attributes, compiling, cite ? function : null, parameters, new Tuple(returnTypes));
                    compilingInterface.callables.Add(compilingFunction);
                    function.compiling = compilingFunction;
                }
            }
            foreach (var file in classes)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingClass = (CompilingClass)declarations!.Find(value => value.name == file.name)!;

                for (int i = 0; i < file.inherits.Count; i++)
                {
                    var type = GetType(context, manager, file.inherits[i]);
                    if (type.dimension > 0) collector.Add(file.inherits[i].name, CErrorLevel.Error, "不能继承数组");
                    if (i > 0 || type.code == TypeCode.Interface)
                    {
                        compilingClass.inherits.Add(type);
                        if (type.code != TypeCode.Interface) collector.Add(file.inherits[i].name, CErrorLevel.Error, "必须是接口");
                    }
                    else
                    {
                        compilingClass.parent = type;
                        if (type.code != TypeCode.Handle) collector.Add(file.inherits[i].name, CErrorLevel.Error, "不能继承该类型");
                    }
                    var inherit = manager.GetSourceDeclaration(type);
                    if (inherit is CompilingInterface @interface) @interface.implements.Add(compilingClass);
                    else if (inherit is CompilingClass @class) @class.implements.Add(compilingClass);
                }
                foreach (var variable in file.variables)
                {
                    var type = GetType(context, manager, variable.type);
                    var declaration = new Declaration(library.name, variable.visibility, DeclarationCategory.ClassVariable, compiling.GetMemberName(file.name.ToString(), variable.name.ToString()), default);
                    var compilingVariable = new CompilingVariable(variable.name, declaration, variable.attributes, compiling, cite ? variable : null, variable.isReadonly, type, variable.expression, relies);
                    compilingClass.variables.Add(compilingVariable);
                    variable.compiling = compilingVariable;
                }
                foreach (var function in file.constructors)
                {
                    var parameters = new List<CompilingCallable.Parameter>();
                    foreach (var parameter in function.parameters)
                        parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                    var parameterTypes = new List<Type>();
                    foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                    var declaration = new Declaration(library.name, function.visibility, DeclarationCategory.Constructor, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingFunction(function.name, declaration, function.attributes, compiling, cite ? function : null, parameters, new Tuple([]), new LogicBlock(document, compilingClass, parameters, [], compiling, function.body, relies, collector));
                    compilingClass.constructors.Add(compilingFunction);
                    function.compiling = compilingFunction;
                }
                foreach (var function in file.functions)
                {
                    var parameters = new List<CompilingCallable.Parameter>();
                    foreach (var parameter in function.parameters)
                        parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                    var parameterTypes = new List<Type>();
                    foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                    var returnTypes = new List<Type>();
                    foreach (var type in function.returns)
                        returnTypes.Add(GetType(context, manager, type));
                    var declaration = new Declaration(library.name, function.visibility, DeclarationCategory.ClassFunction, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingVirtualFunction(function.name, declaration, function.attributes, compiling, cite ? function : null, parameters, new Tuple(returnTypes), new LogicBlock(document, compilingClass, parameters, returnTypes, compiling, function.body, relies, collector));
                    compilingClass.functions.Add(compilingFunction);
                    function.compiling = compilingFunction;
                }
            }
            foreach (var file in delegates)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingDelegate = (CompilingDelegate)declarations!.Find(value => value.name == file.name)!;

                var name = compilingDelegate.declaration.name;
                var parameters = new List<CompilingCallable.Parameter>();
                foreach (var parameter in file.parameters)
                    parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                var parameterTypes = new List<Type>();
                foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                var returnTypes = new List<Type>();
                foreach (var type in file.returns)
                    returnTypes.Add(GetType(context, manager, type));
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Delegate, name, new Tuple(parameterTypes));

                declarations.Remove(compilingDelegate);
                library.delegates.Remove(compilingDelegate);

                var references = compilingDelegate.references;
                compilingDelegate = new CompilingDelegate(file.name, declaration, file.attributes, compiling, cite ? file : null, parameters, new Tuple(returnTypes));
                compilingDelegate.references.AddRange(references);

                declarations.Add(compilingDelegate);
                library.delegates.Add(compilingDelegate);
                file.compiling = compilingDelegate;
            }
            foreach (var file in tasks)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingTask = (CompilingTask)declarations!.Find(value => value.name == file.name)!;

                var name = compilingTask.declaration.name;
                var returnTypes = new List<Type>();
                foreach (var type in file.returns)
                    returnTypes.Add(GetType(context, manager, type));
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Task, name, default);

                declarations.Remove(compilingTask);
                library.tasks.Remove(compilingTask);

                var references = compilingTask.references;
                compilingTask = new CompilingTask(file.name, declaration, file.attributes, compiling, cite ? file : null, new Tuple(returnTypes));
                compilingTask.references.AddRange(references);

                declarations.Add(compilingTask);
                library.tasks.Add(compilingTask);
                file.compiling = compilingTask;
            }
            foreach (var file in natives)
            {
                var parameters = new List<CompilingCallable.Parameter>();
                foreach (var parameter in file.parameters)
                    parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                var parameterTypes = new List<Type>();
                foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                var returnTypes = new List<Type>();
                foreach (var type in file.returns)
                    returnTypes.Add(GetType(context, manager, type));
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Function, compiling.GetChildName(file.name.ToString()), new Tuple(parameterTypes));
                var native = new CompilingNative(file.name, declaration, file.attributes, compiling, cite ? file : null, parameters, new Tuple(returnTypes));
                library.natives.Add(native);
                compiling.AddDeclaration(native);
                file.compiling = native;
            }
        }
    }
}
