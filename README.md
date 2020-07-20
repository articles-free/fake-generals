# 伪generals.io控制台小游戏

RT

**演示视频：**

![](https://i.loli.net/2020/07/20/ymUwpN6BcnrOTiQ.gif)

基本操作与 generals.io 相同，WASD移动兵力，按Z切换半兵，鼠标左键单击可以移动光标。

特别地，按F可以切换不同玩家的视角。（开发者模式）

其中用```{}```包裹的数字表示主基地，用```[]```包裹的数字表示城市，选中的土地会以红色高亮显示。

在夺旗地图中，用```()```包裹的数字表示旗帜，用```<>```包裹的数字表示拿到旗帜的玩家。

**在占点地图中，用```[]```包裹的数字表示据点，敬请注意。**

**TDM模式规则：**

- 队友之间共享视野，但不可互相攻击

- 最后存活的队伍胜利

**吃鸡地图规则：**

- 玩家视野从 3\*3 改为 5\*5

- 道具说明：```+```表示医疗包，可以增加 10 点生命值；```<--l```表示剑，可以增加 10% 的伤害；```[O] ```表示护甲，每个护甲可以抵消 10 点伤害；```[L] ```表示闪光弹（负面效果），使视野降至 1\*1，持续 10 个回合；```[C]```表示药片，可以增加 10 点生命上限；```[F]```表示防毒面具，每个防毒面具可抵消 2% 的毒圈伤害

- 部分只有空投掉落的高级道具：```{+}```表示高级医疗包，等同于三个普通医疗包；```{O}```表示钻石护甲，等同于三个普通护甲；```<==I```表示钻石剑，等同于三个普通剑；```{C}```表示高级药片，等同于三个普通药片；```{2X}```表示二倍镜，使视野增至 7\*7；```{F}```表示高级防毒面具。等同于三个普通防毒面具

- 特别地，如果击杀了某个人，会得到对方拥有的所有剑、护甲和防毒面具，也可以得到对方的二倍镜

- 当生命值小于生命上限时，会持续回血

- 每次毒圈扩散时，都会刷出新道具

**夺旗地图规则：**

- 除了没有毒圈和空投外，与吃鸡地图相同

- 每次有队伍得分时，都会刷出新道具

- 死后可以复活，复活后有 10 个回合的无敌时间，但清除身上的全部道具

- 率先得到 10 分的队伍胜利

**占点地图规则：**

- 除了没有毒圈和空投外，与吃鸡地图相同

- 每次有据点被占领时，都会刷出新道具

- 死后可以复活，复活后有 10 个回合的无敌时间，但清除身上的全部道具，**并扣除玩家所在队伍的 10 点分数**

- **所有道具都只会生成在据点的 7\*7 范围内**

- 率先得到 1000 分的队伍胜利

**几个注意事项：**

- 要求Windows系统。如果输出的中文显示成乱码，可以把系统字符集切换为UTF-8。

提供一个Windows10系统切换系统字符集的教程：

设置中搜索“语言设置” -> 点击右上角的“管理语言设置” -> 点击“更改系统区域设置” -> 勾选“Beta版: 使用 Unicode UTF-8 提供全球语言支持” -> 点击确定后重启计算机。

![](https://i.loli.net/2020/07/05/DQdofSPtXw9pC6Z.jpg)

- 为了方便控制台中的鼠标点击，建议关闭控制台属性中的“快速编辑模式”。

以Windows10为例：

![](https://i.loli.net/2020/07/05/t319QuCN8bjHWPM.jpg)

- 考虑到控制台用地紧张，大于 99 的数字均以单位表示。如 120 被表示成 ```1H```，4600 被表示成 ```5K```。一块土地内最大兵力为 9000，超出部分将被忽略。

- 游戏中请始终保持全屏，否则可能无法显示全部内容。

- 祝您好运

**更新**

- 游戏结束时增加了对话框提示。

- 对沙雕Bot进行优化，使战斗更加激烈。

- 修复了兵力显示的问题。

- 添加了TDM模式。

- 添加了空白地图和迷宫地图。

- 添加了端午地图。

- 添加了吃鸡地图。

- 修复了毒圈扩散的问题。

- 对部分代码进行优化，无需C++14支持。

- 修复了组队系统的问题。

- 添加了夺旗地图。

- 移除了不必要的键位，优化性能。

- 加入了辅助格点，再也不怕眼瞎。

- 优化了部分道具的显示。

- 添加了大地图支持。（最大 99\*99）

- 添加了50v50模式。

- 修复了攻击判定的问题。

- 对地图输出进行优化。

- 添加了空投和部分道具。

- 添加了部分道具。

- 修复了重生后生命值不为 100 的问题。

- 添加了部分道具。

- 修改了玩家主基地的生成规则。

- 添加了部分道具。

- 添加了保存配置文件的功能。

- 添加了占点地图。

- 修复了道具生成的问题。

- 修复了空投剩余时间为负数的问题。

- 添加了50v50模式下的队友提示。

- 修复了地图输出的问题。

- 修复了攻击判定的问题。

- 修复了正常视野不为 3\*3 的问题。

说明：迷宫和端午地图的生成参考了 [https://github.com/By-Ha/Checkmate/blob/master/game/map.js](https://github.com/By-Ha/Checkmate/blob/master/game/map.js)。

**使用方法：**

直接编译运行```generals.cpp```即可。