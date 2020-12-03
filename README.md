基于Reactor模式的事件驱动库。使用c++编写。只支持linux(废话，现在哪还有window的服务器)。 全是head only，所以不需要什么构建方式，直接include就能用。

没有什么优点，纯粹是自己练手的作品，说说缺点，还有自己的想法。

1. 目前只支持单线程。原因是多线程我不熟练，我目前对计算机体系和操作系统知之甚少，当然可以土法炼铜，但绝对无法发挥多线程的威力。
特别是c++这种需要注重资源管理的语言，可能会引入更多的bug

2. 只支持ipv4和TCP。

3. 对于一些临界条件处理不好。比如，对于每个socket，只关注其最基本的读，写事件。至于带外数据，错误，半关闭等事件，还有低水压等，由于我自身水平不高，
对它们一知半解，所以没有引入。

4. 没有定时器，一个没有定时器的网络库，性能就不会高。

5. 功能太少。连端点都没有。。。 获取对端ip地址这些是别想了。

仔细分析这些缺点，得到一件好事和一件坏事。

坏事是，这些缺点都是因为我水平不高导致的。 好事是，我把网络编程的主干给抓住了，所以我清楚自己认识自己哪些方面不足，对于细节的把握我把它交给时间。

因此，这是我第一次尝试写一个可以称为”项目“的东西，我称它为 0.01 。

#此项目的更新计划。

我希望在0.02版本克服以上缺点，首要的是把准确性搞定。0.02版本在初步定在下个学期前搞定。

0.03版本是性能和功能，争取能比肩c++成名已久的各大网络框架。此版本希望在下学期中期前搞定。
