import { BreakpointEvent, InitializedEvent, LoggingDebugSession, StoppedEvent, TerminatedEvent } from '@vscode/debugadapter';
import { DebugProtocol } from '@vscode/debugprotocol';
import * as RainDebug from './DebugConfigurationProvider'
import * as client from './ClientHelper'

export class RainDebugSession extends LoggingDebugSession {
	private helper = new client.ClientHelper(RainDebug.client)
	constructor(protected configuration: RainDebug.RainDebugConfiguration) {
		super();
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
			this.sendEvent(new StoppedEvent('', 123));
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
		RainDebug.client.destroy();
		if (this.configuration.request == 'launch' && RainDebug.debuggedProcess != null && !RainDebug.debuggedProcess.killed) {
			RainDebug.debuggedProcess.kill()
		}
		this.sendResponse(response)
	}
	protected setBreakPointsRequest(response: DebugProtocol.SetBreakpointsResponse, args: DebugProtocol.SetBreakpointsArguments, request?: DebugProtocol.Request): void {
		response.body = {
			breakpoints: [
				{
					verified: false
				}
			]
		}
		this.sendResponse(response)
	}
	
}