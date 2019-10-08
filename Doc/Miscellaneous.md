- 虚拟内存是硬件异常, 硬件地址翻译, 主存, 磁盘文件和内核软件的完美交互, 它为每个进程提供了一个大的, 一致的, 私有的地址空间, 
  - 它将主存看做一个存储在磁盘上的地址空间的高速缓存, 在主存中只保存活动区域, 并根据需要在磁盘和主存间来回传送数据, 通过这种方式高效得使用主存
  - 它为每个进程提供了一致的地址空间, 简化了内存管理
  - 保护每个进程的地址空间不被其他进程破坏, 在PTE中加保护位
 - MMU (Memory Management Unit), 利用存放在主存中的查询表来动态翻译虚拟地址
 - 局部性原则: 在任意时刻, 程序趋向于在一个较小的活动页面集合上工作
 ***
 - (DDS)DirectDraw Surface
 - (DXT1) 压缩比例1 : 8, 1位或0位alpha, 15位RGB
 - (DXT2) 压缩比例1 : 4, 4位alpha, 12位RGB, RGB已与alpha混合
 - (DXT3) 压缩比例1 : 4, 4位alpha, 12位RGB
 - (DXT3) 压缩比例1 : 4, 4位线性插值的alpha, 12位RGB, RGB已与alpha混合
 - (DXT5) 压缩比例1 : 4, 4位线性插值的alpha, 12位RGB
 - 纹理过滤
     - 各向同性: 在所有方向上都相同, 假设所有的mipmap贴图都是方形而不是在某一个轴向上有压缩的长方形或者其他形状, 不考虑纹理的xy轴与屏幕的xy轴的角度
     - 最近点采样: 放大时会有明显色块, 缩小时会有闪烁和锯齿, 在放大时采样速度很快, 但缩小时开销很高, 因为屏幕上相邻的两个像素点, 可能对应纹理上距离很远的点, 会破坏纹理采样时的内存连续性, 导致cache miss
     - 最近点采样+mipmap: 可以缓解缩小时的闪烁和锯齿, 并保证内存连续性, 但不能解决纹理放大时的色块问题
     - 双线性过滤: 选取texel和pixel之间大小最接近的一层mipmap进行采样, 以pixel对应的纹理坐标为中心, 取周围四个像素值, 然后根据距离中心点的距离进行加权平均
     - 三线性过滤: 实际使用中, 一个pixel使用哪一层(或两层)mipmap并不是由pixel覆盖多少个texel来决定的, 而是根据pixel纹理坐标变化率来决定, 变化率越大, 说明pixel覆盖的texel越多, 使用ddx和ddy计算出变化率, 由变化率最大值决定mipmap层, 通常这个值不是整数(LOD), 即落在两层mipmap之间, 选取这两层mipmap进行双线性过滤, 然后再使用LOD进行线性插值
     - 各向异性过滤: 考虑纹理空间中在u和v方向上的比例，当u:v不是1:1时，将会按比例在各方向上采样不同数量的点来计算最终的结果

类型|采样数
 ---|---
 Nearest Point Sampling|1
 Bilinear|4
 Trilinear|8
Anisotropic Filtering 4X|32
Anisotropic Filtering 16X|128   
***

- Mutex :只有拥有互斥对象的线程才有访问公共资源的权限。因为互斥对象只有一个，所以能保证公共资源不会同时被多个线程同时访问, 能够用于多个进程之间线程互斥问题，并且能完美的解决某进程意外终止所造成的“遗弃”问题
- Critical Section: 在任意时刻只允许一个线程对共享资源进行访问，如果有多个线程试图访问公共资源，那么在有一个线程进入后，其他试图访问公共资源的线程将被挂起，并一直等到进入临界区的线程离开，临界区在被释放后，其他线程才可以抢占。它并不是核心对象，不是属于操作系统维护的，而是属于进程维护的
- Condition Variable:
- Socket:
- Semaphore: 允许多个线程在同一时刻访问同一资源，但是需要限制在同一时刻访问此资源的最大线程数目, 信号量也是内核对象
- Event: 通过通知操作的方式来保持线程的同步，还可以方便实现对多个线程的优先级比较的操作, 事件是内核对象
***
 lambda => 仿函数

每个lambda表达式会产生一个闭包(特有的, 匿名且非联合体的)类型的临时对象 (右值)

对于按值传递的捕获列表, 其传递的值在lambda函数定义的时候就已经决定了, 而按引用传递的捕获列表变量, 其传递的值则等于lambda函数调用时的值

允许lambda表达式向函数指针转换, 前提是lambda函数没有捕获任何变量, 且函数指针所示的函数原型, 必须跟lambda函数有相同的调用方式

typedef int (*Func)(int, int);
***
- mutable: 修饰使用在const函数中的成员变量

- explicit: 修饰只有一个参数的构造函数, 禁止隐式类型转换

- voliate: 普通变量会被存进寄存器, 之后每次从寄存器取出, 当变量值更改时, 会同时把新值重新读入寄存器, 声明为voliate之后, cpu每次会从该变量所在内存直接读取
***
- memory_order: 原子类型的变量在线程中总是保持顺序执行的特性(这就要求编译器在每次原子操作后加入memory barrier) 大多数原子操作都可以使用memory_order作为一个参数

Order|作用
-----|----
memory_order_relaxed | 不对执行顺序做任何保证
memory_order_acquire | 本线程中, 所有后续的读操作必须在本条原子操作完成后执行
memory_order_release | 本线程中, 所有之前的写操作完成后才能执行本条原子操作   
memory_order_acq_rel | 同时拥有memory_order_acquire和 memory_order_release
memory_order_consume | 本线程中, 所有后续的有关本原子的操作, 必须在本条原子操作完成之后执行
memory_order_seq_cst | 全部存取按顺序执行(default)
****        
- const-volatile限定符, auto不会从初始化表达式中带走cv-限定符
****

排序|平均|最坏|稳定性
---|---|---|---|
直接插入排序|O(n^2)|O(n^2)|稳定
冒泡排序|O(n^2)|O(n^2)|稳定
简单选择排序|O(n^2)|O(n^2)|不稳定
快速排序|O(nlogn)|O(n^2)|不稳定
堆排序|O(nlogn)|O(nlogn)|不稳定
归并排序|O(nlogn)|O(nlogn)|稳定
Shell排序|由增量序列决定|由增量序列决定|不稳定
基数排序|O(d(2n+r))|O(d(2n+r))|稳定                                

| | |
---|---|
直接插入排序|将第一个元素看做有序的子序列, 随后的元素依次插入到这个有序的字序列中
冒泡排序|
简单选择排序|从序列中依次选择最小的放到正确的位置上
快速排序|任选序列中一个元素(pivot), 所有较它小的元素排在它前面, 所有较它大的元素排在它后面, 再以此为界, 分别对这两部分重复上述过程 
堆排序|(大顶堆, 小顶堆) 将无序序列构建成一个堆, 输出堆顶元素后, 调整剩余元素成为一个新的堆
归并排序|将序列看做n个有序的子序列, 然后两两归并得到n/2个长度为2的子序列, 重复此过程直到得到一个长度为n的序列 (2路归并排序)
Shell排序|先将整个序列分割成若干个子序列, 分别对各子序列进行插入排序, 等序列基本有序时再进行一次插入排序
基数排序|

****
二叉树的性质:
-  第i (i>=1)层上最多有2^(i-1)个节点
- 深度为k(k>=1)的二叉树上节点数最多为2^k- 1
- 任何一颗二叉树, 若它有n0个叶子节点, n2个度为2的节点, 则必存在n0=n2+1 (n = n0+n1+n2, 边数b=n-1, 且b=n1+2n2), 满二叉树是完全二叉树的一种特殊情况

红黑树的性质:
- 每个节点要么是黑色, 要么是红色
- 根节点是黑色
- 每个叶子节点都是黑色
- 每个红色节点的两个子节点都一定是黑色
- 任意一个节点到每个叶子节点的路径都包含数量相同的黑色节点
- O(logn)

****
```python
AStar(Node A, Node B)
{
	openList.add(A);

	while (openList.size() != 0)
	{
		if (openList.contains(B))
		{
			retun Find();
		}

		Node Current = FindMin(openList);
		openList.remove(Current);
		closeList.add(Current);

		List<Node> neighbors = getNeighbors(Current);
		for Node in neighbors
		{
			if cantReach(Node) or closeList.contains(Node)
			{
				continue;
			}

			if (openList.contains(Node))
			{
				if (weight(A至Current至Node) 小于 weight(A至Node))
				{
					Node.parent = Current;
					calcWeight(Node);
				}
			}
			else
			{
				openList.add(Node);
				Node.parent = Current;
			}
		}
	}
}
```

****

||||
--|--|--|
list|双向链表|常量时间从任意位置插入删除, 不支持随机访问
deque|双端队列|支持随机访问,在结尾或起始插入删除复杂度O(1), 其他任意位置插入或删除复杂度O(n)
forward_list|单链表|常量时间从任意位置插入删除, 不支持随机访问, 比list更有存储效率|
set|红黑树|搜索,删除和插入都拥有对数复杂度|
map|红黑树|搜索,删除和插入都拥有对数复杂度|
unorderer_set|哈希表|搜索,删除和插入都拥有常量复杂度|
unordered_map|哈希表|搜索,删除和插入都拥有常量复杂度|
priority_queue|容器适配器|提供常数时间的（默认）最大元素查找，对数代价的插入与释出|