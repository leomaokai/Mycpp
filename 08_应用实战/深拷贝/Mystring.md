```cpp
class Mystring
{
    public:
    Mystring(const char *str=nullptr)//构造函数
    {
        if(str!=nullptr)
        {
            m_data=new char[strlen(str)+1];
            strcpy(this->m_data,str);
        }
        else
        {
            m_data=new char[1];
            *m_data='\0';
        }
    }
    Mystring(const Mystring &other)//拷贝构造函数
    {
        m_data=new char[strlen(other.m_data)+1];//开辟新的空间
        strcpy(m_data,other.m_data);
    }
    Mystring& operator=(const Mstring &other)//赋值重载函数
    {
        if(this==&other)//防止自身赋值
            return *this;
        delete [] m_data;//删除旧的空间
        m_data=new char[strlen(other.m_data)+1];//开辟新的空间
        strcpy(m_data,other.m_data);
        return *this;
    }
    ~Mystring()
    {
        delete [] m_data;
        m_data=nullptr;
    }
    private:
    char *m_data;
};
```

