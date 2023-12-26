#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <bitset>
#include <sstream>
#include "math.h"

using namespace std;

class node
{
public:
    double probability;
    string data;

    node* left;
    node* right;

    node(string d, double p)
    {
        data = d;
        probability = p;
        left = NULL;
        right = NULL;
    }
    node merge_symbols(node right)
    {
        string d = this->data + right.data;
        double p = this->probability + right.probability;
        node newNode(d,p);
        return newNode;
    }

    bool operator<(node rhs)
    {
        return this->probability<rhs.probability;
    }

    bool operator<=(node rhs)
    {
        return this->probability<=rhs.probability;
    }

    bool operator>(node rhs)
    {
        return this->probability>rhs.probability;
    }
};

// function to compute file size
int get_file_size(string file);
// function to convert character to binary
string to_binary(char C);
// function used in heap (adapted from slides)
void heapify(vector<node>& arr, int n, int i);
// function used in heap (adapted from slides)
void build_heap(vector<node>& arr, int n);
// function used in heap (adapted from slides)
void heap_sort(vector<node>& arr, int n);
// function used to get probability map using huffman coding
map<char, double> get_probability(string file_name);
// function used to create and fill nodes with data
vector<node> create_nodes(const map<char, double>& mp);
// function used to get the string code of symbol
void compute_code(node* current, string code, map<char,string>& codes);
// function used to create huffman table
map<char,string> create_huffman_table(vector<node>& nodes);
// function used to compress file using a seperate table file
void compress_file(string input_file);
// function used to compress file into one single file
void compress_file_single_file(string input_file);
// function used to decompress file with huffman table in a seperate file
void decompress_file(string input_file);
// function used to decompress file where huffman table is stored in the same file
void decompress_file_single_file(string input_file);

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cout<<"Error: incorrect arguments passed"<<endl<<endl;
        cout<<"Usage: "<<argv[0]<<" command filename"<<endl;
        cout<<"Command can either be compress_multi or compress_single or decompress_multi or decompress_single"<<endl<<endl;
        cout<<"compress_multi compresses file and stores the table into a seperate file"<<endl;
        cout<<"compress_single compresses the file and stores the table inside the new compresed file"<<endl;
        cout<<"decompress_multi decompresses a file where the table is stored in a seperate file with extension .table"<<endl;
        cout<<"decompress_single decompress a file where the table is stored inside the same file"<<endl<<endl;

        cout<<"Example: "<<argv[0]<<" compress_single test1.txt"<<endl;
    } else {
        if(argv[1] == string("compress"))
        {
            compress_file(argv[2]);
        } else if(argv[1]==string("decompress")) {
            decompress_file(argv[2]);
        } else if(argv[1]==string("compress_single")) {
            compress_file_single_file(argv[2]);
        } else if (argv[1]==string("decompress_single")){
            decompress_file_single_file(argv[2]);
        } else {
            cout<<"Error: incorrect arguments passed"<<endl;
            cout<<"Usage: "<<argv[0]<<" command filename"<<endl;
            cout<<"Command can either be compress_multi or compress_single or decompress_multi or decompress_single"<<endl;
            cout<<"compress_multi compresses file and stores the table into a seperate file"<<endl;
            cout<<"compress_single compresses the file and stores the table inside the new compresed file"<<endl;
            cout<<"decompress_multi decompresses a file where the table is stored in a seperate file with extension .table"<<endl;
            cout<<"decompress_single decompress a file where the table is stored inside the same file"<<endl;

            cout<<"Example: "<<argv[0]<<" compress_single test1.txt"<<endl;
        }
    }

    return 0;
}

int get_file_size(string file)
{
    FILE *p_file = NULL;
    p_file = fopen(file.c_str(),"rb");
    fseek(p_file,0,SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);
    return size;
}

string to_binary(char C)
{
    bitset<CHAR_BIT> temp(C);
    return temp.to_string();
}

void heapify(vector<node>& arr, int n, int i)
{
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n && arr[l] > arr[largest])
        largest = l;

    if (r < n && arr[r] > arr[largest])
        largest = r;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void build_heap(vector<node>& arr, int n)
{
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);
}

void heap_sort(vector<node>& arr, int n)
{
    build_heap(arr, n);

    for (int i = n - 1; i >= 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

map<char, double> get_probability(string file_name)
{
    map<char, double> mp;
    fstream f;
    int totalCharacterCounter;
    totalCharacterCounter = 0;
    f.open(file_name, ios::in);
    string line;
    if(!f.is_open())
    {
        cout<<"Error: problem in opening file"<<endl;
        exit(1);
    }
    mp.insert({'\n', 0});
    while(getline(f, line))
    {
        for(int i = 0; i<line.length(); i++)
        {
            if(mp.find(line[i]) == mp.end())
            {
                mp.insert({line[i], 1});
            }
            else
            {
                mp[line[i]]++;
            }
            totalCharacterCounter++;
        }
        mp['\n']++;
        totalCharacterCounter++;
    }

    f.close();
    for(auto itr = mp.begin(); itr != mp.end(); itr++)
        itr->second /= totalCharacterCounter;

    return mp;

}

vector<node> create_nodes(const map<char, double>& mp)
{
    vector<node> result;
    for(auto itr = mp.begin(); itr != mp.end(); itr++)
    {
        result.push_back(node(string(1,itr->first),itr->second));
    }
    return result;
}

void compute_code(node* current, string code, map<char,string>& codes)
{
    if(current==NULL)
        return;
    if(current->left==NULL && current->right==NULL)
    {
        codes[current->data[0]] = code;
        return;
    }
    compute_code(current->left,code+"1", codes);
    compute_code(current->right,code+"0",codes);
}

map<char,string> create_huffman_table(vector<node>& nodes)
{
    while(nodes.size()>1)
    {
        node* l = new node(nodes[0]);
        node* r = new node(nodes[1]);
        nodes[0] = nodes[0].merge_symbols(nodes[1]);
        nodes[0].left = l;
        nodes[0].right = r;
        nodes.erase(nodes.begin()+1);
        heap_sort(nodes, nodes.size());
    }

    map<char,string> codes;
    compute_code(&nodes[0],"", codes);

    return codes;

}

void compress_file(string input_file)
{
    string input_file_no_ext = input_file.substr(0,input_file.find('.'));
    map<char,double> probability_table = get_probability(input_file);
    vector<node> nodes = create_nodes(probability_table);

    map<char,string> codes = create_huffman_table(nodes);

    double average_code_length = 0;
    double entropy = 0;
    double probability;
    for(auto itr = codes.begin(); itr != codes.end(); itr++)
    {
        probability = probability_table[itr->first];
        average_code_length += itr->second.length() * probability;
        entropy += probability*log2(1/probability);
    }

    double inputfilesize = get_file_size(input_file);

    ifstream infile;
    ofstream outfile;
    infile.open(input_file);
    if(!infile.is_open())
    {
        cout<<"Error: problem in opening files"<<endl;
        exit(1);
    }

    char symbol;
    string output = "";
    while(infile.get(symbol))
    {
        output += codes[symbol];
    }

    outfile.open(input_file_no_ext+".table");
    outfile<<output.length()<<endl;
    for(auto itr=codes.begin(); itr != codes.end(); itr++)
    {
        if(itr->first=='\n')
            outfile<<"\\n"<<":"<<itr->second<<endl;
        else
            outfile<<itr->first<<":"<<itr->second<<endl;
    }
    outfile.close();

    for(int i=0; i<(output.length()%CHAR_BIT); i++)
        output += "0";

    outfile.open(input_file_no_ext+".compressed", ios_base::binary);
    const unsigned int BitsPerByte = CHAR_BIT;
    unsigned char byte;
    int outputfilesize = output.size()/8;
    while(output.length()>0)
    {
        for(int i=0; i<CHAR_BIT; i++)
        {
            byte <<= 1;
            if(output[i]=='1')
            {
                byte |= 1;
            }
        }
        output = output.substr(CHAR_BIT, output.length());
        outfile<<byte;
        byte = 0;
    }

    infile.close();
    outfile.close();
    cout<<"File compressed successfully to "<<input_file_no_ext<<".compressed with compression table in "<<input_file_no_ext<<".table"<<endl;
    cout<<"average code length: "<<average_code_length<<endl;
    cout<<"efficiency: "<<entropy/average_code_length *100<<endl;
    cout<<"entropy: "<<entropy<<endl;
    cout<<"input file size: "<<inputfilesize<<endl;
    cout<<"output file size: "<<outputfilesize<<endl;
    double compression_ratio = outputfilesize/inputfilesize;
    cout<<"compression ratio: "<<compression_ratio<<endl;
}

void compress_file_single_file(string input_file)
{
    string input_file_no_ext = input_file.substr(0,input_file.find('.'));
    map<char,double> probability_table = get_probability(input_file);
    vector<node> nodes = create_nodes(probability_table);

    map<char,string> codes = create_huffman_table(nodes);

    double average_code_length = 0;
    double entropy = 0;
    double probability;
    for(auto itr = codes.begin(); itr != codes.end(); itr++)
    {
        probability = probability_table[itr->first];
        average_code_length += itr->second.length() * probability;
        entropy += probability*log2(1/probability);
    }

    double inputfilesize = get_file_size(input_file);

    ifstream infile;
    ofstream outfile;
    infile.open(input_file);
    if(!infile.is_open())
    {
        cout<<"Error: problem in opening files"<<endl;
        exit(1);
    }

    char symbol;
    string output = "";
    while(infile.get(symbol))
    {
        output += codes[symbol];
    }
    int count = output.size();

    string temp = to_binary('#');
    outfile.open(input_file_no_ext+".single", ios_base::binary);
    for(auto itr=codes.begin(); itr != codes.end(); itr++)
    {
        temp += to_binary(itr->first);
        temp += itr->second;
        temp += to_binary(';');
    }
    temp += to_binary('$');
    temp += to_binary('$');
    output = temp + output;

    output = bitset<32>(count).to_string() + output;
    for(int i=0; i<(output.length()%CHAR_BIT); i++)
        output += "0";

    const unsigned int BitsPerByte = CHAR_BIT;
    unsigned char byte;
    int outputfilesize = output.size()/8;
    while(output.length()>0)
    {
        for(int i=0; i<CHAR_BIT; i++)
        {
            byte <<= 1;
            if(output[i]=='1')
            {
                byte |= 1;
            }
        }
        output = output.substr(CHAR_BIT, output.length());
        outfile<<byte;
        byte = 0;
    }

    infile.close();
    outfile.close();
    cout<<"File compressed successfully to "<<input_file_no_ext<<".single"<<endl;
    cout<<"average code length: "<<average_code_length<<endl;
    cout<<"efficiency: "<<entropy/average_code_length *100<<endl;
    cout<<"entropy: "<<entropy<<endl;
    cout<<"input file size: "<<inputfilesize<<endl;
    cout<<"output file size: "<<outputfilesize<<endl;
    double compression_ratio = outputfilesize/inputfilesize;
    cout<<"compression ratio: "<<compression_ratio<<endl;
}

void decompress_file(string input_file)
{
    string input_file_no_ext = input_file.substr(0,input_file.find('.'));

    ifstream infile,infiletable;
    ofstream outfile;
    infile.open(input_file, ios::binary);
    infiletable.open(input_file_no_ext+".table");
    outfile.open(input_file_no_ext+".txt");
    if(!infile.is_open() || !outfile.is_open())
    {
        cout<<"Error: problem in opening files"<<endl;
        exit(1);
    }

    // load data from table
    map<string, char> reverse_codes;
    string line, word;
    vector<string> row;
    int count;
    infiletable>>count;
    getline(infiletable, line);
    while(getline(infiletable, line))
    {
        stringstream s(line);
        while(getline(s, word,':'))
            row.push_back(word);
        if(row.size()==2) {
            reverse_codes[row[1]] = (row[0]=="\\n")?'\n':row[0][0];
        }
        row.clear();
    }
    infiletable.close();

    string input = "";
    char c;
    while (infile.get(c))
    {
        for (int i = 7; i >= 0; i--)
            input += (((c >> i) & 1)==1)?"1":"0";
    }

    string result = "";
    string code = "";
    for(int i=0; i<count; i++)
    {
        code += input[i];
        if(reverse_codes.find(code)!=reverse_codes.end())
        {
            result += reverse_codes[code];
            code = "";
        }
    }
    outfile<<result;

    infile.close();
    outfile.close();

    cout<<"File decompressed successfully to "<<input_file_no_ext<<".txt"<<endl;
}

void decompress_file_single_file(string input_file)
{
    string input_file_no_ext = input_file.substr(0,input_file.find('.'));

    ifstream infile,infiletable;
    ofstream outfile;
    infile.open(input_file, ios::binary);
    outfile.open(input_file_no_ext+".txt");
    if(!infile.is_open() || !outfile.is_open())
    {
        cout<<"Error: problem in opening files"<<endl;
        exit(1);
    }

    // load data from table
    map<string, char> reverse_codes;
    string line, word;
    vector<string> row;
    int count;

    string input = "";
    char c;
    while (infile.get(c))
    {
        for (int i = 7; i >= 0; i--)
            input += (((c >> i) & 1)==1)?"1":"0";
    }
    count = stoi(input.substr(0,input.find("00100011")),0,2);
    string table = input.substr(input.find("00100011")+8,input.find("0010010000100100")-input.find("00100011")-8);
    while(table.size()>0)
    {
        string row = table.substr(0, table.find("00111011"));
        string letter_binary = row.substr(0,8);
        char letter = static_cast<char>(std::stoi(letter_binary, nullptr, 2));
        string code = row.substr(8,row.size());
        reverse_codes[code] = letter;
        table = table.substr(table.find("00111011")+8,table.size());
    }

    input = input.substr(input.find("0010010000100100")+16, input.size());
    string result = "";
    string code = "";
    for(int i=0; i<count; i++)
    {
        code += input[i];
        if(reverse_codes.find(code)!=reverse_codes.end())
        {
            result += reverse_codes[code];
            code = "";
        }
    }
    outfile<<result;

    infile.close();
    outfile.close();

    cout<<"File decompressed successfully to "<<input_file_no_ext<<".txt"<<endl;
}