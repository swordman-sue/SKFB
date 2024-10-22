// 消除 STLPort 对内存布局修改而产生的警告
#pragma warning(disable: 4103)

//取消C4996警告
#pragma warning(disable: 4996)

// 视无效比较操作为一个错误
#pragma warning(error: 4553)
// 视不是所有路径都有返回值为一个错误
#pragma warning(error: 4715)
// 视条件表达式中存在赋值为一个错误
#pragma warning(error: 4706)
// 视变量不初始化就使用为一个错误
#pragma warning(error: 4700)
// 视递归调用时无终止条件为一个错误
#pragma warning(error: 4717)
