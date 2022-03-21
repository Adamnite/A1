 #pragma once


 #include <string>
 using std::string;

 class Source
 {
public:
        Source()
        {
          filename = "{empty_file}";
          contents = "";
        }
        Source(string filename, bool isOutput);

        string getFilename() {return filename;}


        string getDirPath();

        const string& getContents() {return contents;}

        string getBoxedString();

        inline int size()
        {
            return contents.size();

        }
        inline string substr(size_t start, size_t len)
        {
          return contents.substr(start,len)
        }
  private:
      string filename;
      string contents;
 };
