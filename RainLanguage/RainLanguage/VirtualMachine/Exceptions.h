#pragma once

constexpr auto EXCEPTION_NULL_REFERENCE = TEXT("空引用");
constexpr auto EXCEPTION_INVALID_TASK = TEXT("无效的任务");
constexpr auto EXCEPTION_OUT_OF_RANGE = TEXT("越界");
constexpr auto EXCEPTION_INVALID_TYPE = TEXT("无效的类型");
constexpr auto EXCEPTION_NOT_ENUM = TEXT("不是枚举");
constexpr auto EXCEPTION_NOT_ARRAY = TEXT("不是数组");
constexpr auto EXCEPTION_NOT_DELEGATE = TEXT("不是委托");
constexpr auto EXCEPTION_NOT_TASK = TEXT("不是任务");
constexpr auto EXCEPTION_NOT_DELEGATE_OR_TASK = TEXT("不是委托或任务");
constexpr auto EXCEPTION_TASK_NOT_UNSTART = TEXT("任务状态不是未执行");
constexpr auto EXCEPTION_TASK_NOT_RUNNING = TEXT("任务状态不是执行中");
constexpr auto EXCEPTION_TASK_NOT_COMPLETED = TEXT("任务状态不是已完成");
constexpr auto EXCEPTION_DIVIDE_BY_ZERO = TEXT("除零");
constexpr auto EXCEPTION_INVALID_CAST = TEXT("无效的类型转换");
constexpr auto EXCEPTION_STACK_OVERFLOW = TEXT("栈溢出");
constexpr auto EXCEPTION_ASSIGNMENT_READONLY_VARIABLE = TEXT("赋值只读变量");
constexpr auto EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH = TEXT("参数列表不匹配");
constexpr auto EXCEPTION_RETURN_LIST_DOES_NOT_MATCH = TEXT("返回值列表不匹配");
constexpr auto EXCEPTION_IGNORE_WAIT_BUT_CONDITION_NOT_VAILD = TEXT("忽略等待的任务中出现了等待一个条件不成立的语句");
constexpr auto EXCEPTION_IGNORE_WAIT_BUT_TASK_NOT_COMPLETED = TEXT("忽略等待的任务中出现了等待一个未完成的任务");