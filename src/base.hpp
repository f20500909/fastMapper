#ifndef SRC_BASE_HPP
#define SRC_BASE_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <regex>
#include <boost/algorithm/string.hpp>


using namespace std;


class Svg {
public:
    Svg() {

    }

    void parseData(string path) {
        auto tmp = get_svg_data(path);
        parseDataMap(tmp);
    }


    void parseDataMap(vector<string> strVector) {
        //截取到有效片段
        for (int i = 0; i < strVector.size(); i++) {
            string row = strVector[i];
            int len = row.size();
            strVector[i] = row.substr(8, len - 8 - 2);
            std::cout << strVector[i] << std::endl;
        }

//    将有效片段分割
        for (int i = 0; i < strVector.size(); i++) {
            std::vector<std::string> vecSegTag;
            vector<vector<float >> singlePolylineVec;

            std::string &singlePolylineStr = strVector[i];
            boost::split(vecSegTag, singlePolylineStr, boost::is_any_of((" ,")));

            for (int j = 0; j < vecSegTag.size(); j = j + 2) {
                singlePolylineVec.push_back({static_cast<float>(atof(vecSegTag[j].c_str())),
                                             static_cast<float>(atof(vecSegTag[j + 1].c_str()))});
            }
            m[i] = singlePolylineVec;
        }
    }

    vector<string> get_svg_data(string str) {
        str = "(points)=\"[\\s\\S]*?\"";

        fstream in(str);
        string svg_context((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

        regex reg(str);
        smatch sm;
        vector<string> res;

        while (regex_search(svg_context, sm, reg)) {
            res.push_back(sm.str());
            svg_context = sm.suffix();
        }
        return res;
    }

    /**
    * Transform a 2D array containing the patterns id to a 2D array containing the pixels.
    * 将包含2d图案的id数组转换为像素数组
    */
    virtual Matrix<Cell> to_image(const Matrix<unsigned> &output_patterns) = 0;

    virtual void write_image_png(const std::string &file_path, const Matrix<Cell> &m);

    virtual void showResult(Matrix<unsigned> mat);


private:
    std::unordered_map<int, vector<vector<float>>> m;
};

class Base {
public:
    Base(const Options &op) : option(op), _direction(op.directionSize) {
    }

    Direction _direction;
    //svg解码器
//    Svg _svg;
    const Options option;

    bool isVaildCoordinate(coordinate coor) {
        int x = coor[0];
        int y = coor[1];
        if (x < 0 || x >= (int) option.wave_width) {
            return false;
        }
        if (y < 0 || y >= (int) option.wave_height) {
            return false;
        }
        return true;
    }

public:

};

#endif // SRC_DASE_HPP