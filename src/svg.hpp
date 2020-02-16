#ifndef SRC_SVG_HPP
#define SRC_SVG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "Data.hpp"


template<class T>
class Svg : public Data<T> {
public:
    Svg(const Options op) : Data<T>(op) {
        parseData();
//        initPatterns();
//        generateCompatible();
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

private:
    std::unordered_map<int, vector<vector<float>>> m;
};
#endif //SRC_SVG_HPP
