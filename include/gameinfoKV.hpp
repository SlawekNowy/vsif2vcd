

#include <vector>
#include <string>

namespace FileSystem {
    
#if 0
    struct gameInfoKV
    {


        std::basic_string<char> name;
        std::vector<std::pair<std::basic_string<char>, std::basic_string<char>>> attribs;
        std::vector<std::pair<std::basic_string<char>, std::shared_ptr<gameInfoKV>>> childs;

        void add_attribute(std::basic_string<char> key, std::basic_string<char> value)
        {
            attribs.emplace_back(key,value);
        }
        void add_child(std::unique_ptr<gameInfoKV> child)
        {
            std::shared_ptr<gameInfoKV> obj{ child.release() };
            //childs.emplace(obj->name, obj);
            childs.emplace_back(obj->name,obj);
        }
        void set_name(std::basic_string<char> n)
        {
            name = std::move(n);
        }
    };

#endif

    struct gameInfoKV
    {
        std::string name;
        std::vector<std::pair<std::string, std::string>> attribs;
        std::vector<std::pair<std::string, std::shared_ptr<gameInfoKV>>> childs;
        void add_attribute(std::string key, std::string value)
        {
            attribs.emplace_back(key, value);
        }
        void add_child(std::unique_ptr< gameInfoKV > child)
        {
            std::shared_ptr<gameInfoKV> obj{ child.release() };
            //childs.emplace(obj->name, obj);
            childs.emplace_back(obj->name, obj);
        }
        void set_name(std::string n)
        {
            name = std::move(n);
        }
    };
}