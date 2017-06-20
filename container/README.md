
# SyncQueue
`condition_variable` + `mutex`用来同步和通知。内存存储使用`vector`，只需一次分配空间。向队列添加`Put`可以传递右值参数。

## 测试
测试代码test/test_syncqueue.cpp。
24核服务器，CPU主频2.10GHz。队列大小为50，50个线程`Put`，50个线程`Take`，`Put` `Take`各1e8(1亿)次，总共耗时208s。