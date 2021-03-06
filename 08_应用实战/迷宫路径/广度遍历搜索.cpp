#include <iostream>
#include <queue>
#include <vector>
using namespace std;

/*
寻找迷宫最短路径
在迷宫里面怎么找最短的路径？？？
广度遍历迷宫路径搜索
0 0 1 1 1 1
1 0 0 0 0 1
1 0 1 1 0 1
1 0 0 0 0 1
1 0 1 1 1 1
1 0 0 0 0 0

arry[row*col];
深度遍历搜索需要用到的是栈或递归
而广度遍历搜索需要用到的是队列
1.判断方向任然是 右 下 左 上
2.将第一个节点插入队列,判断右方向节点是否可走,若可走,将右方节点插入队列...
3.直到节点四个方向都判断结束,然后出队列
4.继续判断队列头部的节点
5.当队列头部节点坐标为出口时,即找到最短路径

但是队列并不能保存路径节点,需要利用一个数组记录节点坐标
数组索引对应一个节点的位置(x*row+y),数组内容为到达这个节点的节点坐标
例如:
在6*6迷宫中
最后一个节点坐标为(5,5),其对应数组索引为[5*6+5]=[35],其中的内容可能是坐标为(5,4)的节点
(5,5)->[35](5,4)->[34](5,3)->[33](5,2)->[32](5,1)->[31](4,1)->
			[23](3,1)->[19](2,1)->[13](1,1)->[7](0,1)->[1](0,0)
牺牲一定的空间和时间换取最短路径
*/


// 定义方向
const int RIGHT = 0;
const int DOWN = 1;
const int LEFT = 2;
const int UP = 3;
const int WAY_NUM = 4;

// 定义行走状态
const int YES = 4;
const int NO = 5;

// 迷宫
class Maze
{
public:
	Maze(int row, int col)
		:_row(row)
		, _col(col)
	{
		_pMaze = new Node * [_row];
		for (int i = 0; i < _row; ++i)
		{
			_pMaze[i] = new Node[_col];
		}
		//节点坐标于节点在数组中的位置转换
		// node._x*_row + node._y
		_pPath.resize(_row * _col);
	}

	void initNode(int x, int y, int val)
	{
		_pMaze[x][y]._x = x;
		_pMaze[x][y]._y = y;
		_pMaze[x][y]._val = val;
		for (int i = 0; i < WAY_NUM; ++i)
		{
			_pMaze[x][y]._state[i] = NO;
		}
	}

	void setNodeState()
	{
		for (int i = 0; i < _row; ++i)
		{
			for (int j = 0; j < _col; ++j)
			{
				if (_pMaze[i][j]._val == 1)
				{
					continue;
				}

				if (j < _col - 1 && _pMaze[i][j + 1]._val == 0)
				{
					_pMaze[i][j]._state[RIGHT] = YES;
				}

				if (i < _row - 1 && _pMaze[i + 1][j]._val == 0)
				{
					_pMaze[i][j]._state[DOWN] = YES;
				}

				if (j > 0 && _pMaze[i][j - 1]._val == 0)
				{
					_pMaze[i][j]._state[LEFT] = YES;
				}

				if (i > 0 && _pMaze[i - 1][j]._val == 0)
				{
					_pMaze[i][j]._state[UP] = YES;
				}
			}
		}
	}

	void searchMazePath()
	{
		if (_pMaze[0][0]._val == 1)
		{
			return;
		}
		_queue.push(_pMaze[0][0]);

		while (!_queue.empty())
		{
			Node front = _queue.front();
			int x = front._x;
			int y = front._y;

			// 右方向
			if (_pMaze[x][y]._state[RIGHT] == YES)
			{
				_pMaze[x][y]._state[RIGHT] = NO;
				_pMaze[x][y + 1]._state[LEFT] = NO;
				// 在辅助数组中记录一下节点的行走信息
				_pPath[x * _row + y + 1] = _pMaze[x][y];
				_queue.push(_pMaze[x][y + 1]);
				//检查是否为出口
				if (check(_pMaze[x][y + 1]))
					return;
			}

			// 下方向
			if (_pMaze[x][y]._state[DOWN] == YES)
			{
				_pMaze[x][y]._state[DOWN] = NO;
				_pMaze[x + 1][y]._state[UP] = NO;
				_pPath[(x + 1) * _row + y] = _pMaze[x][y];
				_queue.push(_pMaze[x + 1][y]);
				if (check(_pMaze[x + 1][y]))
					return;
			}

			// 左方向
			if (_pMaze[x][y]._state[LEFT] == YES)
			{
				_pMaze[x][y]._state[LEFT] = NO;
				_pMaze[x][y - 1]._state[RIGHT] = NO;
				_pPath[x * _row + y - 1] = _pMaze[x][y];
				_queue.push(_pMaze[x][y - 1]);
				if (check(_pMaze[x][y - 1]))
					return;
			}

			// 上方向
			if (_pMaze[x][y]._state[UP] == YES)
			{
				_pMaze[x][y]._state[UP] = NO;
				_pMaze[x - 1][y]._state[DOWN] = NO;
				_pPath[(x - 1) * _row + y] = _pMaze[x][y];
				_queue.push(_pMaze[x - 1][y]);
				if (check(_pMaze[x - 1][y]))
					return;
			}

			// 出队列
			_queue.pop();
		}
	}

	void showMazePath()
	{
		if (_queue.empty())
		{
			cout << "不存在一条迷宫路径！" << endl;
		}
		else
		{
			// 回溯寻找迷宫路径节点
			int x = _row - 1;
			int y = _col - 1;
			for (;;)
			{
				_pMaze[x][y]._val = '*';
				if (x == 0 && y == 0)
					break;
				Node node = _pPath[x * _row + y];
				x = node._x;
				y = node._y;
			}

			for (int i = 0; i < _row; ++i)
			{
				for (int j = 0; j < _col; ++j)
				{
					if (_pMaze[i][j]._val == '*')
					{
						cout << "* ";
					}
					else
					{
						cout << _pMaze[i][j]._val << " ";
					}
				}
				cout << endl;
			}
		}
	}
private:
	// 定义迷宫节点路径信息
	struct Node
	{
		int _x;
		int _y;
		int _val; // 节点的值
		int _state[WAY_NUM]; // 记录节点四个方向的状态
	};

	// 检查是否是右下角的迷宫出口节点
	bool check(Node& node)
	{
		return node._x == _row - 1 && node._y == _col - 1;
	}

	Node** _pMaze;
	int _row;
	int _col;
	queue<Node> _queue; // 广度遍历依赖的队列结构
	vector<Node> _pPath; // 记录广度优先遍历时，节点的行走信息
};

int main()
{
	cout << "请输入迷宫的行列数(例如：10 10):";
	int row, col, data;
	cin >> row >> col;

	Maze maze(row, col); // 创建迷宫对象

	cout << "请输入迷宫的路径信息(0表示可以走，1表示不能走):" << endl;
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			cin >> data;
			// 可以初始化迷宫节点的基本信息
			maze.initNode(i, j, data);
		}
	}

	// 开始设置所有节点的四个方

	//向的状态
	maze.setNodeState();

	// 开始从左上角搜索迷宫的路径信息了
	maze.searchMazePath();

	// 打印迷宫路径搜索的结果
	maze.showMazePath();

	return 0;
}