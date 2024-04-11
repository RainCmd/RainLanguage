import { InitializedEvent, LoggingDebugSession, Scope, StoppedEvent, TerminatedEvent, Variable } from '@vscode/debugadapter';
import { DebugProtocol } from '@vscode/debugprotocol';
import * as RainDebug from './DebugConfigurationProvider'
import * as client from './ClientHelper'

enum StepType {
	// 逐过程
	Over = 2,
	// 单步
	Into,
	// 跳出
	Out,
}
class SpaceNode{
	public parent: SpaceNode = null
	public children: SpaceNode[] = null
	public variables: VariableNode[] = null
	constructor(public id: number, public name: string) { }
	public has(node: VariableNode) {
		for (let index = node.GetRoot().space; index; index = index.parent){
			if (index == this) {
				return true
			}
		}
		return false
	}
	public GetNames() {
		const result: string[] = []
		for (let index: SpaceNode = this; index.parent; index = index.parent) {
			result.push(index.name)	
		}
		return result.reverse()
	}
}
class VariableNode {
	public space: SpaceNode = null
	public parent: VariableNode = null
	public index: number
	public members: VariableNode[] = null
	constructor(public id: number, public name: string, public type: string, public value: string) { }
	public GetRoot() {
		let index: VariableNode = this;
		while (index.parent) {
			index = index.parent
		}
		return index
	}
	public GetMemberIndies() {
		const indies: number[] = []
		for (let index: VariableNode = this; index.parent; index = index.parent){
			indies.push(index.index)
		}
		return indies.reverse()
	}
}

export class RainDebugSession extends LoggingDebugSession {
	private helper = new client.ClientHelper(RainDebug.client)
	constructor(protected configuration: RainDebug.RainDebugConfiguration) {
		super();
	}
	private GetRelativePath(value: string): string {
		return value.substring(this.configuration.projectPath.length).replace(/^[\\/]*/gi, "")
	}
	protected attachRequest(response: DebugProtocol.AttachResponse, args: DebugProtocol.AttachRequestArguments, request?: DebugProtocol.Request): void {
		console.log("attach")
		this.sendResponse(response)
	}
	protected launchRequest(response: DebugProtocol.LaunchResponse, args: DebugProtocol.LaunchRequestArguments, request?: DebugProtocol.Request): void {
		console.log("lanuch")
		this.sendResponse(response)
	}

	protected initializeRequest(response: DebugProtocol.InitializeResponse, args: DebugProtocol.InitializeRequestArguments): void {
		RainDebug.client.on('close', () => {
			this.sendEvent(new TerminatedEvent())
		})
		this.helper.on(client.Proto.SEND_OnBreak, reader => {
			const threadId = Number(reader.ReadLong())
			this.sendEvent(new StoppedEvent("命中断点", threadId));
		}).on(client.Proto.SEND_OnException, reader => {
			const threadId = Number(reader.ReadLong())
			this.sendEvent(new StoppedEvent("exception", threadId, reader.ReadString()));
		}).on(client.Proto.SEND_Message, reader => {
			const msg = reader.ReadString()
			console.log(msg)
		})
		response.body = {
			//调试适配器支持' configurationDone '请求。
			supportsConfigurationDoneRequest: false,
			//调试适配器支持函数断点。
			supportsFunctionBreakpoints: false,
			//调试适配器支持条件断点。
			supportsConditionalBreakpoints: false,
			//调试适配器支持在达到指定次数后中断执行的断点。
			supportsHitConditionalBreakpoints: false,
			//调试适配器支持(无副作用)“评估”数据请求徘徊。
			supportsEvaluateForHovers: true,
			//' setExceptionBreakpoints '请求的可用异常过滤器选项。
			exceptionBreakpointFilters: [
				{
					filter: '异常信息',
					label: "推出信息需要包含的字符串",
					description: `只有当推出信息包含输入的字符串时时才会引发断点`,
					default: true,
					supportsCondition: true,
					conditionDescription: `输入退出信息`
				}
			],
			//调试适配器支持通过' stepBack '和' reverseconcontinue '请求后退。
			supportsStepBack: false,
			//调试适配器支持将变量设置为值。
			supportsSetVariable: true,
			//调试适配器支持重新启动帧。
			supportsRestartFrame: false,
			//调试适配器支持' gotoTargets '请求。
			supportsGotoTargetsRequest: false,
			//调试适配器支持' stepInTargets '请求。
			supportsStepInTargetsRequest: false,
			//调试适配器支持' completion '请求。
			supportsCompletionsRequest: false,
			//在REPL中应该触发完成的一组字符。如果没有指定，UI应该假定为'。'字符。
			completionTriggerCharacters: [".", "["],
			//调试适配器支持' modules '请求。
			supportsModulesRequest: false,
			//由调试适配器公开的附加模块信息集。
			additionalModuleColumns: [],
			//调试适配器支持的校验和算法。
			supportedChecksumAlgorithms: [],
			//调试适配器支持“重启”请求。在这种情况下，客户端不应该通过终止和重新启动适配器来实现“重启”，而是通过调用“重启”请求来实现。
			supportsRestartRequest: false,
			//调试适配器支持' setExceptionBreakpoints '请求上的' exceptionOptions '。
			supportsExceptionOptions: false,
			//调试适配器在' stackTrace '、' variables '和' evaluate '请求上支持' format '属性。
			supportsValueFormattingOptions: false,
			//调试适配器支持' exceptionInfo '请求。
			supportsExceptionInfoRequest: true,
			//调试适配器支持' disconnect '请求上的' terminateDebuggee '属性。
			supportTerminateDebuggee: true,
			//调试适配器支持' disconnect '请求上的' suspendDebuggee '属性。
			supportSuspendDebuggee: true,
			//调试适配器支持堆栈部分的延迟加载，这需要同时支持' startFrame '和' levels '参数以及' stackTrace '请求的' totalFrames '结果。
			supportsDelayedStackTraceLoading: false,
			//调试适配器支持' loaddsources '请求。
			supportsLoadedSourcesRequest: false,
			//调试适配器通过解释' SourceBreakpoint '的' logMessage '属性来支持日志点。
			supportsLogPoints: false,
			//调试适配器支持' terminateThreads '请求。
			supportsTerminateThreadsRequest: true,
			//调试适配器支持' setExpression '请求。
			supportsSetExpression: false,
			//调试适配器支持“终止”请求。
			supportsTerminateRequest: true,
			//调试适配器支持数据断点。
			supportsDataBreakpoints: false,
			//调试适配器支持' readMemory '请求。
			supportsReadMemoryRequest: true,
			//调试适配器支持' writemmemory '请求。
			supportsWriteMemoryRequest: false,
			//调试适配器支持“反汇编”请求。
			supportsDisassembleRequest: false,
			//调试适配器支持“取消”请求。
			supportsCancelRequest: true,
			//调试适配器支持' breakpointLocations '请求。
			supportsBreakpointLocationsRequest: false,
			//调试适配器支持' evaluate '请求中的' clipboard '上下文值。
			supportsClipboardContext: false,
			//调试适配器支持步进请求的步进粒度(参数'粒度')。
			supportsSteppingGranularity: false,
			//调试适配器支持根据指令引用添加断点。
			supportsInstructionBreakpoints: false,
			//调试适配器支持' filterOptions '作为' setExceptionBreakpoints '请求的参数。
			supportsExceptionFilterOptions: true,
			//调试适配器在执行请求(' continue '， ' next '， ' stepIn '， ' stepOut '， ' reverseContinue '， ' stepBack ')上支持' singleThread '属性。
			supportsSingleThreadExecutionRequests: true,
			//调试适配器支持的断点模式，如“硬件”或“软件”。如果存在，客户端可能允许用户选择一种模式，并将其包含在其' setBreakpoints '请求中。
			//客户端可以将此数组中的第一个适用模式作为设置断点的手势中的“默认”模式。
			breakpointModes: []
		}
		this.sendResponse(response)
		this.sendEvent(new InitializedEvent())
	}

	protected disconnectRequest(response: DebugProtocol.DisconnectResponse, args: DebugProtocol.DisconnectArguments, request?: DebugProtocol.Request): void {
		this.helper.Send(new client.Writer(client.Proto.RECV_Close))
		RainDebug.client.destroy();
		if (this.configuration.request == 'launch' && RainDebug.debuggedProcess != null && !RainDebug.debuggedProcess.killed) {
			RainDebug.debuggedProcess.kill()
		}
		this.sendResponse(response)
	}
	private breakpointMap = new Map<string, number[]>()
	protected async setBreakPointsRequest(response: DebugProtocol.SetBreakpointsResponse, args: DebugProtocol.SetBreakpointsArguments, request?: DebugProtocol.Request): Promise<void> {
		const path = this.GetRelativePath(args.source.path)
		let lines = this.breakpointMap.get(path)
		if (lines == undefined) {
			lines = []
		}
		const removed = lines.filter(v => !args.lines.includes(v))
		if (removed.length > 0) {
			const removeReq = new client.Writer(client.Proto.RECV_RemoveBreaks);
			removeReq.WriteString(path)
			removeReq.WriteUint(removed.length)
			removed.forEach(value => removeReq.WriteUint(value))
			this.helper.Send(removeReq)
		}
		const added = args.lines
		if (added.length > 0) {
			const addReq = new client.Writer(client.Proto.RRECV_AddBreadks)
			addReq.WriteUint(request.seq)
			addReq.WriteString(path)
			addReq.WriteUint(added.length)
			added.forEach(value => addReq.WriteUint(value))
			const addRes = await this.helper.Request(request.seq, addReq)
			let cnt = addRes.ReadInt()
			let invalidLines: number[] = []
			while (cnt-- > 0) {
				invalidLines.push(addRes.ReadInt());
			}
			response.body = { breakpoints: [] }
			args.lines.forEach(v => {
				response.body.breakpoints.push({
					verified: !invalidLines.includes(v)
				})
			});
		}
		this.breakpointMap.set(path, args.lines)
		this.sendResponse(response)
	}
	protected setExceptionBreakPointsRequest(response: DebugProtocol.SetExceptionBreakpointsResponse, args: DebugProtocol.SetExceptionBreakpointsArguments, request?: DebugProtocol.Request): void {
		//todo 设置异常断点，
		this.sendResponse(response)
	}
	protected async threadsRequest(response: DebugProtocol.ThreadsResponse, request?: DebugProtocol.Request): Promise<void> {
		let req = new client.Writer(client.Proto.RRECV_Tasks)
		req.WriteUint(request.seq)
		let res = await this.helper.Request(request.seq, req)
		let cnt = res.ReadInt()
		response.body = { threads: [] }
		while (cnt-- > 0) {
			let id = res.ReadLong()
			response.body.threads.push({
				id: Number(id),
				name: "TaskID:" + id.toString()
			})
		}
		this.sendResponse(response);
	}
	private threadId: bigint
	private traceDeep: number
	protected async stackTraceRequest(response: DebugProtocol.StackTraceResponse, args: DebugProtocol.StackTraceArguments, request?: DebugProtocol.Request): Promise<void> {
		let req = new client.Writer(client.Proto.RRECV_Task)
		req.WriteUint(request.seq)
		req.WriteUint(args.threadId)
		let res = await this.helper.Request(request.seq, req)
		const stacks: DebugProtocol.StackFrame[] = []
		let cnt = res.ReadInt()
		for (let index = 0; index < cnt; index++) {
			const file = res.ReadString()
			stacks.push({
				id: index,
				name: file,
				line: res.ReadInt(),
				source: {
					path: this.configuration.projectPath + "\\" + file
				},
				column: 0
			})
		}
		response.body = {
			stackFrames: stacks,
			totalFrames: stacks.length
		}
		this.threadId = BigInt(args.threadId)
		this.traceDeep = stacks.length - args.startFrame - 1;
		this.sendResponse(response)
	}

	private referenceIndex = 0
	private localVariable: SpaceNode = null
	private globalVariable: SpaceNode = null
	private spaceMap = new Map<number, SpaceNode>()
	private variableMap = new Map<number, VariableNode>()
	private CreateSpaceNode(name: string) {
		const result = new SpaceNode(this.referenceIndex++, name)
		this.spaceMap.set(result.id, result)
		return result
	}
	private CreateVariable(name: string, structured: boolean, type: string, value: string) {
		if (structured) {
			const result = new VariableNode( this.referenceIndex++, name, type, value)
			this.variableMap.set(result.id, result)
			return result
		}
		return new VariableNode(0, name, type, value)
	}
	protected scopesRequest(response: DebugProtocol.ScopesResponse, args: DebugProtocol.ScopesArguments, request?: DebugProtocol.Request): void {
		this.spaceMap.clear()
		this.variableMap.clear()
		this.localVariable = this.CreateSpaceNode("局部变量")
		this.globalVariable = this.CreateSpaceNode(this.configuration.projectName)
		response.body = {
			scopes: [
				new Scope(this.localVariable.name, this.localVariable.id, false),
				new Scope(this.globalVariable.name, this.globalVariable.id, true)
			]
		}
		this.sendResponse(response)
	}

	private ReadVariableMembers(src: VariableNode, variables: Variable[], reader: client.Reader) {
		let count = reader.ReadInt()
		src.members = []
		while (count-- > 0) {
			const node = this.CreateVariable(reader.ReadString(), reader.ReadBool(), reader.ReadString(), reader.ReadString())
			node.parent = src
			node.index = src.members.length
			src.members.push(node)
			variables.push({
				name: node.name,
				variablesReference: node.id,
				value: node.value
			})
		}
	}
	private ReadSpaceVariables(space: SpaceNode, variables: Variable[], reader: client.Reader) {
		space.variables = []
		let count = reader.ReadInt()
		while (count-- > 0) {
			const node = this.CreateVariable(reader.ReadString(), reader.ReadBool(), reader.ReadString(), reader.ReadString())
			node.space = space
			space.variables.push(node)
			variables.push({
				name: node.name,
				variablesReference: node.id,
				value: node.value
			})
		}
	}
	protected async variablesRequest(response: DebugProtocol.VariablesResponse, args: DebugProtocol.VariablesArguments, request?: DebugProtocol.Request): Promise<void> {
		const variables: Variable[] = []
		const space = this.spaceMap.get(args.variablesReference)
		if (space == this.localVariable) {
			const req = new client.Writer(client.Proto.RRECV_Trace)
			req.WriteUint(request.seq)
			req.WriteLong(this.threadId)//threadId
			req.WriteUint(this.traceDeep)//traceDeep
			const res = await this.helper.Request(request.seq, req)
			this.ReadSpaceVariables(space, variables, res)
		} else if (space) {
			const req = new client.Writer(client.Proto.RRECV_Space)
			req.WriteUint(request.seq)
			const names = space.GetNames()
			req.WriteUint(names.length)
			names.forEach(value => req.WriteString(value))
			const res = await this.helper.Request(request.seq, req)
			space.children = []
			let count = res.ReadInt()
			while (count-- > 0) {
				const node = this.CreateSpaceNode(res.ReadString())
				node.parent = space
				space.children.push(node)
				variables.push({
					name: node.name,
					variablesReference: node.id,
					value: ""
				})
			}
			this.ReadSpaceVariables(space, variables, res)
		}
		const variable = this.variableMap.get(args.variablesReference)
		if (variable) {
			if (this.localVariable.has(variable)) {
				const req = new client.Writer(client.Proto.RRECV_Local)
				req.WriteUint(request.seq)
				req.WriteLong(this.threadId)//threadId
				req.WriteUint(this.traceDeep)//traceDeep
				req.WriteString(variable.name)
				const indies = variable.GetMemberIndies()
				req.WriteUint(indies.length)
				indies.forEach(value => req.WriteUint(value))
				const res = await this.helper.Request(request.seq, req)
				this.ReadVariableMembers(variable, variables, res)
			} else if(this.globalVariable.has(variable)) {
				const req = new client.Writer(client.Proto.RRECV_Global)
				req.WriteUint(request.seq)
				req.WriteLong(this.threadId)//threadId
				req.WriteUint(this.traceDeep)//traceDeep
				req.WriteString(variable.name)
				const names = variable.GetRoot().space.GetNames()
				req.WriteUint(names.length)
				names.forEach(value => req.WriteString(value))
				const indies = variable.GetMemberIndies()
				req.WriteUint(indies.length)
				indies.forEach(value => req.WriteUint(value))
				const res = await this.helper.Request(request.seq, req)
				this.ReadVariableMembers(variable, variables, res)
			} else {
				const req = new client.Writer(client.Proto.RRECV_Hover)
				req.WriteUint(request.seq)
				req.WriteLong(this.threadId)//taskId
				req.WriteUint(this.traceDeep)//traceDeep
				req.WriteString(this.hoverFile)
				req.WriteUint(this.hoverLine)
				req.WriteUint(this.hoverChar)
				const res = await this.helper.Request(request.seq, req)
				this.ReadVariableMembers(variable, variables, res)
			}
		}
		response.body.variables = variables
		this.sendResponse(response)
	}
	protected pauseRequest(response: DebugProtocol.PauseResponse, args: DebugProtocol.PauseArguments, request?: DebugProtocol.Request): void {
		this.helper.Send(new client.Writer(client.Proto.RECV_Pause))
		this.sendResponse(response)
	}
	protected continueRequest(response: DebugProtocol.ContinueResponse, args: DebugProtocol.ContinueArguments, request?: DebugProtocol.Request): void {
		this.helper.Send(new client.Writer(client.Proto.RECV_Continue))
		this.sendResponse(response)
	}
	protected nextRequest(response: DebugProtocol.NextResponse, args: DebugProtocol.NextArguments, request?: DebugProtocol.Request): void {
		const req = new client.Writer(client.Proto.RECV_Step)
		req.WriteUint(StepType.Over)
		this.helper.Send(req)
		this.sendResponse(response)
	}
	protected stepInRequest(response: DebugProtocol.StepInResponse, args: DebugProtocol.StepInArguments, request?: DebugProtocol.Request): void {
		const req = new client.Writer(client.Proto.RECV_Step)
		req.WriteUint(StepType.Into)
		this.helper.Send(req)
		this.sendResponse(response)
	}
	protected stepOutRequest(response: DebugProtocol.StepOutResponse, args: DebugProtocol.StepOutArguments, request?: DebugProtocol.Request): void {
		const req = new client.Writer(client.Proto.RECV_Step)
		req.WriteUint(StepType.Out)
		this.helper.Send(req)
		this.sendResponse(response)
	}
	private hoverFile: string
	private hoverLine: number
	private hoverChar: number
	protected async evaluateRequest(response: DebugProtocol.EvaluateResponse, args: DebugProtocol.EvaluateArguments, request?: DebugProtocol.Request): Promise<void> {
		if (args.context == 'hover') {
			let array = args.expression.split(' ')
			if (array.length == 3) {
				const req = new client.Writer(client.Proto.RRECV_Eval)
				req.WriteUint(request.seq)
				req.WriteLong(this.threadId)//taskId
				req.WriteUint(this.traceDeep)//traceDeep
				this.hoverFile = array[0]
				this.hoverLine = Number(array[1])
				this.hoverChar = Number(array[2])
				req.WriteString(this.hoverFile)
				req.WriteUint(this.hoverLine)
				req.WriteUint(this.hoverChar)
				const res = await this.helper.Request(request.seq, req)
				if (res.ReadBool()) {
					const node = this.CreateVariable("", res.ReadBool(), "", res.ReadString())
					response.body = {
						variablesReference: node.id,
						result: node.value
					}
				} else {
					response.success = false
				}
			}
		}
		this.sendResponse(response)
	}
}