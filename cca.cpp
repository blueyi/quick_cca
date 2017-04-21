/*
 * cc.cpp
 * Copyright (C) 2017 dutoeserver <dutoeserver@A0835-Server-2>
 *
 * Distributed under terms of the MIT license.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>

const int DES = 49;
const int ZERO = 48;
void vec_print(const std::vector<std::vector<int>> &vec)
{
    int line = 0;
    for (const auto &v : vec) {
        std::cout << line++ << ": ";
        for (auto num : v)
            std::cout << num;
        std::cout << std::endl;
    }
}

class FindIceBlock
{
public:
    static void fileToVec(std::vector<std::vector<int>> &vec, const std::string &fileName)
    {
        std::ifstream inf(fileName);
        if (!inf)
            std::cout << "ERROR: " << fileName << " Open failed!" << std::endl;
        std::string str;
        while(std::getline(inf, str)) {
            vec.push_back(std::vector<int> (str.begin(), str.end() - 1));
//            std::istringstream istr(str);
//            std::istream_iterator<int> in_initer(istr), leof;
//            vec.push_back(std::vector<int> (in_initer, leof));
        }
    }

    static void vecToFile(const std::vector<std::vector<int>> &vec, const std::string &fileName)
    {
        std::ofstream ofs(fileName);
        if (!ofs)
            std::cout << "ERROR: " << fileName << " Open failed!" << std::endl;
        for (const auto &v : vec) {
            for (auto num : v) {
                ofs << num;
            }
            ofs << std::endl;
        }
    }

    /// <summary>
    /// 遍历并标记所有冰块,返回冰块数量+1,即最大冰块像素在resultArr中的序号
    /// </summary>
    /// <param name="resultArr">需要处理的数组,1表示冰，0表示非冰</param>
    /// <param name="resultSum">存放所有冰块序号及对应的大小</param>
    /// <param name="width">图像的宽</param>
    /// <param name="height">图像的高</param>
    /// <param name="allIceNum">冰的总数,即所有1的总数</param>
    /// <param name="maxIce">最大冰块面积</param>
    /// <param name="minIce">最小冰块面积</param>
    /// <returns>返回冰块数量+1,即最大冰块序号</returns>
    static int findIce(std::vector<std::vector<int>> &resultArr, std::vector<int> &resultSum, int &maxIce, int &minIce)
    {

        int width = (resultArr[0]).size();
        int height = resultArr.size();
        int allIceNumHere = 0;
        //            bool[] isChecked = new bool[allIceNum]; //检查该冰点是否已被标记
        int iceOrder = 1;  //冰块顺序
        int iceOrderSum = 0;   //当前冰块序号对应的冰块大小
        int arrLength = width * height;
        std::vector<int> currentCheckedIceIdx(arrLength);   //用于存放当前检查过的像素索引
        std::vector<int> arrLine(arrLength);  //存放一维化之后的数组
        int lastPoint = 0; //上一块冰的起点

        //将像素线性化
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                arrLine[j * width + i] = resultArr[i][j] - '0';
                if (resultArr[i][j] == DES)
                    ++allIceNumHere;
            }
        }
        std::cout << "-- Total: " << allIceNumHere << "---" << std::endl;

        while (allIceNumHere != 0)
        {
            int idxUp = 0;  //向上扫描的起始坐标
            int idxDown = 0;  //向下扫描的起始坐标
            int currentOrderStart = 0;  //记录当前序号冰块的检查起点

            currentCheckedIceIdx.clear();

            iceOrder++;
            iceOrderSum = 0;  //当前冰块序号对应的所有冰数量

            //找到需要标记的冰,即像素为1的点
            findFirstPoint(lastPoint, arrLine, arrLength);
            idxDown = idxUp = lastPoint;

            //确定当前冰块的初始扫描行
            markIceLine(arrLine, iceOrder, iceOrderSum, idxUp, currentCheckedIceIdx, width, allIceNumHere);

            //检查是否当前连通块的冰全部标记完成
            while (!isAllCheckedInCurrentIceBlock(currentCheckedIceIdx, arrLine, currentOrderStart, idxUp, idxDown, width, height, iceOrderSum))
            {
                //重要的边界------
                while ((idxUp >= 0) && (arrLine[idxUp] == 1))
                {
                    markIceLine(arrLine, iceOrder, iceOrderSum, idxUp, currentCheckedIceIdx, width, allIceNumHere);
                    idxUp = idxUp - width;
                }
                //重要的边界------
                while ((idxDown / width < height) && (arrLine[idxDown] == 1))
                {
                    markIceLine(arrLine, iceOrder, iceOrderSum, idxDown, currentCheckedIceIdx, width, allIceNumHere);
                    idxDown = idxDown + width;
                }
            }
            resultSum.push_back(iceOrderSum);
            if (iceOrder == 2)
            {
                maxIce = minIce = iceOrderSum;
            }
            else
            {
                if (maxIce < iceOrderSum)
                    maxIce = iceOrderSum;
                if (minIce > iceOrderSum)
                    minIce = iceOrderSum;
            }
        }

        //将修改后的结果写入到原数组
        int idxX = 0;
        int idxY = 0;
        for (int idx = 0; idx < arrLength; idx++)
        {
            idxX = idx % width;
            idxY = idx / width;
            resultArr[idxX][idxY] = arrLine[idx];
        }

        return iceOrder;
    }

    /// <summary>
    ///标记当前行中所有需要标记的冰块
    /// </summary>
    /// <param name="arrLine">线性化的二维数组</param>
    /// <param name="iceOrder">当前冰块的序号</param>
    /// <param name="iceOrderSum">当前序号的冰块总像素数量</param>
    /// <param name="lastPoint">标记的起点</param>
    /// <param name="currentCheckedIceIdx">存放当前序号对应的已经检查过的像素位置索引</param>
    /// <param name="width">图像宽度</param>
    /// <param name="allIceNumHere">冰的总像素数</param>
    static void markIceLine(std::vector<int> &arrLine, int iceOrder, int &iceOrderSum, int lastPoint, std::vector<int> &currentCheckedIceIdx, int width, int &allIceNumHere)
    {
        int leftX = lastPoint;
        int rightX = lastPoint + 1;
        while (true)
        {
            arrLine[leftX] = iceOrder;
            currentCheckedIceIdx[iceOrderSum] = leftX;  //记录当前冰块坐标
            iceOrderSum++;
            allIceNumHere--;
            leftX--;

            //重要的边界------
            //满足以下条件退出：超出图像左边界, 该位置不是冰，该文件是标记过的冰
            if ((leftX < 0) || (((leftX + 1) % width) == 0) || (arrLine[leftX] != 1))
                break;
        }

        while (true)
        {
            //重要的边界------
            //满足以下条件退出：超出图像左边界, 该位置不是冰，该文件是标记过的冰
            if ((rightX > int(arrLine.size())) || ((rightX % width) == 0) || (arrLine[rightX] != 1))  //如果向右已经到达图像右边界，则直接退出
                break;

            arrLine[rightX] = iceOrder;
            currentCheckedIceIdx[iceOrderSum] = rightX;  //记录当前冰块坐标
            iceOrderSum++;
            allIceNumHere--;
            rightX++;
        }

    }

    /// <summary>
    ///每块冰第一次被扫描到时，确定出它的初始位置
    /// </summary>
    /// <param name="lastPoint">上次扫描的终点</param>
    /// <param name="arrLine">线性化之后的原二维冰块数组</param>
    /// <param name="arrLength">像素总数，即width*height</param>
    static void findFirstPoint(int &lastPoint, const std::vector<int> &arrLine, int arrLength)
    {
        int i = lastPoint;
        while (i < arrLength)
        {
            if (arrLine[i] == 1)
                break;
            i++;
        }
        lastPoint = i;
    }

    /// <summary>
    ///判断是否当前冰块中的所有像素都已经被扫描过, 如果不是，则确定出下一次需要扫描的起点,并返回false，检查8个位置
    /// </summary>
    /// <param name="currentCheckedIceIdx">存放当前序号对应的已经检查过的像素位置索引</param>
    /// <param name="arrLine">线性化之后的原二维冰块数组</param>
    /// <param name="currentOrderStart">记录当前序号冰块的检查起点，该值始终指向最后一个被检查到需要被标记的像素位置</param>
    /// <param name="idxUp">向上扫描的起始坐标</param>
    /// <param name="idxDown">向下扫描的起始坐标</param>
    /// <param name="width">图像的宽</param>
    /// <param name="height">图像的高</param>
    /// <param name="iceOrderSum">当前序号的冰块总像素数量</param>
    /// <returns>如果当前冰块中的所有像素已经扫描完成则返回ture,否则返回false</returns>
    static bool isAllCheckedInCurrentIceBlock(const std::vector<int> &currentCheckedIceIdx, std::vector<int> &arrLine, int &currentOrderStart, int &idxUp, int &idxDown, int width, int height, int iceOrderSum)
    {
        bool checkDone = true;
        int x, y;
        int idxX, idxY;
        idxX = idxY = 0;
        int idx = currentOrderStart;

        for (; idx < iceOrderSum; idx++)
        {
            x = currentCheckedIceIdx[idx] % width;
            y = currentCheckedIceIdx[idx] / width;
            if ((x - 1 >= 0) && arrLine[x - 1 + y * width] == 1)
            {
                checkDone = false;
                idxX = x - 1;
                idxY = y;
                break;
            }

            if ((x + 1 < width) && arrLine[x + 1 + y * width] == 1)
            {
                checkDone = false;
                idxX = x + 1;
                idxY = y;
                break;
            }

            if ((y - 1 >= 0) && arrLine[x + (y - 1) * width] == 1)
            {
                checkDone = false;
                idxX = x;
                idxY = y - 1;
                break;
            }

            if ((y + 1 < height) && arrLine[x + (y + 1) * width] == 1)
            {
                checkDone = false;
                idxX = x;
                idxY = y + 1;
                break;
            }

            if ((x - 1 >= 0) && (y - 1 >= 0) && arrLine[x - 1 + (y - 1) * width] == 1)
            {
                checkDone = false;
                idxX = x - 1;
                idxY = y - 1;
                break;
            }

            if ((x + 1 < width) && (y - 1 >= 0) && arrLine[x + 1 + (y - 1) * width] == 1)
            {
                checkDone = false;
                idxX = x + 1;
                idxY = y - 1;
                break;
            }

            if ((x + 1 < width) && (y + 1 < height) && arrLine[x + 1 + (y + 1) * width] == 1)
            {
                checkDone = false;
                idxX = x + 1;
                idxY = y + 1;
                break;
            }

            if ((x - 1 >= 0) && (y + 1 < height) && arrLine[x - 1 + (y + 1) * width] == 1)
            {
                checkDone = false;
                idxX = x - 1;
                idxY = y + 1;
                break;
            }

        }

        currentOrderStart = idx + 1;  //记录当前冰块中下一次的起点
        idxUp = idxY * width + idxX;
        if (idxY + 1 < height)
            idxDown = (idxY + 1) * width + idxX;
        return checkDone;
    }

};

int main(void)
{
    std::string fin = "ori_board.txt";
    std::string fot = "res_board.txt";
    std::vector<std::vector<int>> oriVec;
    FindIceBlock::fileToVec(oriVec, fin);
    std::vector<int> resSum(2, 0);
    int maxIce, minIce;
    FindIceBlock::findIce(oriVec, resSum, maxIce, minIce);
//    vec_print(oriVec);
    FindIceBlock::vecToFile(oriVec, fot);
    std::cout << "---result---" << std::endl;
    int res_total = 0;
    for (std::size_t it = 0; it < resSum.size(); ++it) {
        if (resSum[it] > 5)
            std::cout << it << ": " << resSum[it] << std::endl;
        res_total += resSum[it];
    }
    std::cout << "---Total result sum---" << std::endl;
    std::cout << res_total << std::endl;
    return 0;
}
