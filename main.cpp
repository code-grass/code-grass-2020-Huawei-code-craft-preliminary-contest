/***********************
author:code-grass
**********************/

#include<iostream>
using namespace std;
#include<vector>
#include<time.h>
#include<algorithm>
#include<unordered_map>
#include<fstream>
#include<queue>

//#define TESTCODE   //��������
#define SUBMITCODE   //�ύ����


#ifdef TESTCODE  //��������
#define ReadFilePath "F:\\aa_��Ϊ�����ս��\\src\\test_data_28w.txt"  
#define SaveFilePath "result_my_28w.txt"
#endif // TESTCODE

#ifdef SUBMITCODE //�ύ����
#define ReadFilePath "/data/test_data.txt"
#define SaveFilePath "/projects/student/result.txt"
#endif // SUBMITCODE

#define RecordNumber 280000
#pragma warning(disable:4996)
//g++ -O3 main.cpp -o test -lpthread

typedef unsigned int uint;

//ѭ��ת��·����
class Path
{
public:
	Path() {}
	Path(uint length, vector<uint>& path) : m_Length(length), m_Path(path) {}

	bool operator<(const Path& path)
	{
		if (this->m_Length != path.m_Length)
		{
			return this->m_Length < path.m_Length;
		}
		else
		{
			for (uint i = 0; i < this->m_Length; i++)
			{
				if (this->m_Path[i] != path.m_Path[i])
					return this->m_Path[i] < path.m_Path[i];
			}
		}
	}

	uint m_Length; //ѭ��ת��·������
	vector<uint> m_Path;  //ѭ��ת��·��
};

class Solution
{
public:
	Solution()
	{
		m_InputFile.reserve(RecordNumber * 2);  //Ԥ���ռ�
	}

	void ReadFile()
	{
		FILE* fp = fopen(ReadFilePath, "r"); //ֻ����ʽ��ȡ�ļ�
#ifdef TESTCODE
		if (fp == nullptr)
		{
			cout << "�ļ���ʧ��" << endl;
			system("pause");
			exit(0);
		}
#endif // TESTCODE
		uint count = 0;
		uint id1, id2, money;
		while (fscanf(fp, "%u,%u,%u", &id1, &id2, &money) != EOF)
		{
			//cout << id1 << " " << id2 << endl;
			this->m_InputFile.emplace_back(id1);
			this->m_InputFile.emplace_back(id2);
			count++;
		}
		//this->m_RecordNum = count;
#ifdef TESTCODE
		cout << "The number of transfer record is: " << count << endl;
#endif // TESTCODE
	}

	void CreateGraph()
	{
		auto tempData = m_InputFile;
		sort(tempData.begin(), tempData.end());
		tempData.erase(unique(tempData.begin(), tempData.end()), tempData.end());
		m_IDInOrder = tempData;

		//m_IDNum = tempData.size();

		m_IDNum = 0;
		for (auto &data : m_IDInOrder)
		{
			m_IDHash[data] = m_IDNum++;
		}

		m_LGraph = vector<vector<uint>>(m_IDNum);
		m_InDgree = vector<uint>(m_IDNum, 0);
		m_OutDgree = vector<uint>(m_IDNum, 0);
		uint id1Pos = 0, id2Pos = 0;
		uint inputFileSize = m_InputFile.size();
		for (uint i = 0; i < inputFileSize; i += 2)
		{
			id1Pos = m_IDHash[m_InputFile[i]];
			id2Pos = m_IDHash[m_InputFile[i + 1]];
			m_LGraph[id1Pos].emplace_back(id2Pos);
			m_OutDgree[id1Pos]++;
			m_InDgree[id2Pos]++;
		}
	}

	//ȥ�������ԭ���޷��γɻ����˻�  ת���˻�����
	void TopologySort()
	{
		queue<uint> queueTemp;
		for (uint i = 0; i < m_IDNum; i++)
		{
			if (m_InDgree[i] == 0)
				queueTemp.push(i);
		}
		while (!queueTemp.empty())
		{
			uint temp = queueTemp.front();
			queueTemp.pop();
			for (uint &id : m_LGraph[temp])
			{
				if (--m_InDgree[id] == 0)
					queueTemp.push(id);
			}
		}
		uint count = 0;
		for (uint i = 0; i < m_IDNum; i++)
		{
			if (m_InDgree[i] == 0)
			{
				m_LGraph[i].clear();
				count++;
			}
		}
#ifdef TESTCODE
		cout << "�Ƴ����Ϊ��ĵ�" << count << "��" << endl;
#endif // TESTCODE		

		for (auto &id : m_LGraph)   //��ÿ���˻���ת���˻����򣬷����������ת���˻��д�����ʼ�˻���λ��
		{
			if (id.size() > 1)
				sort(id.begin(), id.end());
		}
	}


	void TraversalTwoLayer()
	{
		m_TwoLayerResult = vector<unordered_map<uint, vector<uint>>>(m_IDNum);
		for (uint startID = 0; startID < m_IDNum; startID++)
		{
			for (auto &middleID : m_LGraph[startID])
			{
				for (auto &headID : m_LGraph[middleID])
				{
					if (middleID > headID && startID > headID)
						m_TwoLayerResult[headID][startID].emplace_back(middleID);
				}
			}
		}
	}

	void solve()
	{
		TopologySort();
		TraversalTwoLayer();
		Path path;
		m_IsPass = vector<bool>(m_IDNum, false);
		for (uint i = 0; i < m_IDNum; i++)
		{
			if (!m_LGraph[i].empty())
			{
				DFS(i, i, 1, path);
			}

		}
		sort(m_Result.begin(), m_Result.end());
	}

	void DFS(uint startIdPos, uint currentPos, uint depth, Path& path)
	{
		m_IsPass[currentPos] = true;
		path.m_Path.emplace_back(currentPos);
		//��ǰ�˻���ת���˻��д��ڵ���ͷ�ڵ��λ��
		auto rightPos = lower_bound(m_LGraph[currentPos].begin(), m_LGraph[currentPos].end(), startIdPos);
		if (rightPos != m_LGraph[currentPos].end() && *rightPos == startIdPos && depth < 7 && depth >= 3)
		{
			path.m_Length = depth;
			m_Result.emplace_back(path);
		}
		if (depth < 6)
		{
			for (; rightPos != m_LGraph[currentPos].end(); rightPos++)
			{
				if (!m_IsPass[*rightPos])
					DFS(startIdPos, *rightPos, depth + 1, path);
			}
		}
		else if (depth == 6)
		{
			unordered_map<uint, vector<uint>>::iterator it = m_TwoLayerResult[startIdPos].find(currentPos);
			if (it != m_TwoLayerResult[startIdPos].end())
			{
				for (uint &id : it->second)
				{
					if (m_IsPass[id])
						continue;
					vector<uint> pathTemp = path.m_Path;
					pathTemp.emplace_back(id);
					m_Result.emplace_back(Path(7, pathTemp));
				}
			}
		}
		m_IsPass[currentPos] = false;
		path.m_Path.pop_back();
	}

	void SaveFile_C()    //��C���Է�ʽд��
	{
		uint SaveFileNum = m_Result.size();
		FILE* fw = fopen(SaveFilePath, "w");
		fprintf(fw, "%u\n", SaveFileNum);
		for (auto &path : m_Result)
		{
			switch (path.m_Length)
			{
			case 3:
				fprintf(fw, "%u,%u,%u\n", m_IDInOrder[path.m_Path[0]], m_IDInOrder[path.m_Path[1]], m_IDInOrder[path.m_Path[2]]);
				break;
			case 4:
				fprintf(fw, "%u,%u,%u,%u\n", m_IDInOrder[path.m_Path[0]], m_IDInOrder[path.m_Path[1]], m_IDInOrder[path.m_Path[2]], m_IDInOrder[path.m_Path[3]]);
				break;
			case 5:
				fprintf(fw, "%u,%u,%u,%u,%u\n", m_IDInOrder[path.m_Path[0]], m_IDInOrder[path.m_Path[1]], m_IDInOrder[path.m_Path[2]], m_IDInOrder[path.m_Path[3]], m_IDInOrder[path.m_Path[4]]);
				break;
			case 6:
				fprintf(fw, "%u,%u,%u,%u,%u,%u\n", m_IDInOrder[path.m_Path[0]], m_IDInOrder[path.m_Path[1]], m_IDInOrder[path.m_Path[2]], m_IDInOrder[path.m_Path[3]], m_IDInOrder[path.m_Path[4]], m_IDInOrder[path.m_Path[5]]);
				break;
			case 7:
				fprintf(fw, "%u,%u,%u,%u,%u,%u,%u\n", m_IDInOrder[path.m_Path[0]], m_IDInOrder[path.m_Path[1]], m_IDInOrder[path.m_Path[2]], m_IDInOrder[path.m_Path[3]], m_IDInOrder[path.m_Path[4]], m_IDInOrder[path.m_Path[5]], m_IDInOrder[path.m_Path[6]]);
				break;
			default:
				break;
			}
		}
		fclose(fw);
	}

	void SaveFile_Stream()
	{
		ofstream ofs(SaveFilePath, ios::out);
		ofs << m_Result.size() << endl;
		for (auto &path : m_Result)
		{
			uint length = path.m_Path.size();
			ofs << m_IDInOrder[path.m_Path[0]];
			for (uint i = 1; i < length; i++)
			{
				ofs << "," << m_IDInOrder[path.m_Path[i]];
			}
			ofs << endl;
		}
		ofs.close();
	}


private:
	vector<uint> m_InputFile;  //��Ŷ�ȡ�������� id1��id2
	//uint m_RecordNum;
	vector<uint> m_IDInOrder;  //��� ����� ���˻�ID
	unordered_map<uint, uint> m_IDHash; //���<ID�ţ���Ӧ���������>
	uint m_IDNum;  //ID��
	vector<vector<uint>> m_LGraph;  //��� �˻�ID��� ���ڽӱ�
	vector<uint> m_InDgree;  //���
	vector<uint> m_OutDgree;  //����
	vector<unordered_map<uint, vector<uint>>> m_TwoLayerResult;  //���ȱ������� ���vector����-ת���˻� key-ת���˻�(ʵ��Ϊͷ�ڵ�) �ڲ�vector-�м��˻�
	//unordered_map<uint, vector<vector<uint>>> m_TwoLayerResult;  //���ȱ������� key-ת���˻� vector����ֵ-ת���˻� �ڲ�vector-�м��˻�
	vector<bool> m_IsPass;  // �Ƿ񾭹��˶�Ӧ�� ID���
	vector<Path> m_Result;  //�������ļ�
};

int main()
{
#ifdef TESTCODE
	clock_t stop, start, startEarly;
	double duration;

	startEarly = start = clock();
	Solution solution;
	solution.ReadFile();
	stop = clock();
	duration = double(stop - start) / CLK_TCK;
	cout << "read file time: " << duration << "s" << endl;

	start = stop;
	solution.CreateGraph();
	stop = clock();
	duration = double(stop - start) / CLK_TCK;
	cout << "create graph time: " << duration << "s" << endl;

	start = stop;
	solution.solve();
	stop = clock();
	duration = double(stop - start) / CLK_TCK;
	cout << "solve time: " << duration << "s" << endl;

	start = stop;
	solution.SaveFile_C();
	stop = clock();
	duration = double(stop - start) / CLK_TCK;
	cout << "save file time: " << duration << "s" << endl;

	duration = double(stop - startEarly) / CLK_TCK;
	cout << "total time: " << duration << "s" << endl;

	system("pause");
#else
	Solution solution;
	solution.ReadFile();
	solution.CreateGraph();
	solution.solve();
	solution.SaveFile_C();
#endif // TESTCODE

	
	return 0;
}