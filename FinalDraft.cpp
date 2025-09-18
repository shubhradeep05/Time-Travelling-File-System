//
// Created by Shubhradeep on 05-09-2025.
//
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

//Tree implementation
class TreeNode {
public:
    // Version (TreeNode) Structure
    int version_id;
    string content;
    string message;
    time_t created_timestamp;
    time_t snapshot_timestamp;
    TreeNode* parent;
    vector<TreeNode*> children;
    bool is_snapshot;
    TreeNode(const int id,const string& s = "", TreeNode* par = nullptr) {
        version_id = id;
        message = "";
        content = s;
        parent = par;
        created_timestamp = time(nullptr);
        //last_modified = time(nullptr);
        snapshot_timestamp = time(0);
        is_snapshot = false;
    }
};

//HashMap implementation
template<typename k,typename v>
class HashMap {
public:
    class node {
        public:
        k key;
        v value;
        node* next;

        node(const k& kk, const v& vv) {
            key = kk;
            value = vv;
            next = nullptr;
        }
    };
    int cap;
    int size;
    vector<node*> buckets;
    int hash(const int &kkey) {
        int result = (kkey * 251) % cap;
        return result<0 ? result+cap : result;
    }
    int hash(const string& key) {
        int hash_value = 0;
        for (char c : key) {
            hash_value = (hash_value*31+c) % cap;
        }
        return hash_value<0 ? hash_value+cap : hash_value;
    }
    HashMap(int initial_size = 10000) {
        cap = initial_size;
        size = 0;
        buckets.resize(cap,nullptr);
    }

    ~HashMap() {
        for (int i = 0; i < cap; i++) {
            node* current = buckets[i];
            while (current) {
                node* temp = current;
                current = current->next;
                delete temp;
            }
        }
    }
    void insert(const k& ke, const v& va) {
        int ind = hash(ke);
        node* current = buckets[ind];
        while (current) {
            if (current->key == ke) {
                current->value = va;
                return;
            }
            current = current->next;
        }

        node* new_node = new node(ke, va);
        new_node->next = buckets[ind];
        buckets[ind] = new_node;
        size++;
    }
    v* find(const k &ke) {
        int ind = hash(ke);
        node* current = buckets[ind];
        while (current) {
            if (current->key == ke) {
                return &(current->value);
            }
            current = current->next;
        }
        return nullptr;
    }

    void remove(const k& ke) {
        int ind = hash(ke);
        node* current = buckets[ind];
        node* prev = nullptr;

        while (current) {
            if (current->key == ke) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    buckets[ind] = current->next;
                }
                delete current;
                size--;
                return;
            }
            prev = current;
            current = current->next;
        }
    }
};
//heap
//element used to make the heap more general
template<typename k,typename v>
class Element {
    public:
    k key;
    v value;
    Element(k kk,v vv) {
        key = kk;
        value = vv;
    }
};
// using template so that it will handle both time and versions

template<typename k,typename v>
class MaxHeap {
private:

    vector<Element<k,v>> arr;
    HashMap<k,int> hash;
    void swap_elements(int i, int j) {
        swap(arr[i], arr[j]);
        hash.insert(arr[i].key, i);
        hash.insert(arr[j].key, j);
    }
    int parent(int index) {
        return (index - 1) / 2;
    }
    int left(int index) {
        return 2 * index+1;
    }
    int right(int index) {
        return 2 * index+2;
    }
    void heapify_up(int index) {
        while (index > 0) {
            int par = parent(index);
            if (arr[index].value < arr[par].value) break;
            swap_elements(index, par);
            index = par;
        }
    }

    void heapify_down(int index) {
        int n = arr.size();
        while (true) {
            int l = left(index);
            int r = right(index);
            int largest = index;
            if (l < n && arr[l].value > arr[largest].value) {
                largest = l;
            }
            if (r < n && arr[r].value > arr[largest].value) {
                largest = r;
            }
            if (largest == index) {
                break;
            }
            swap_elements(index, largest);
            index = largest;
        }
    }

public:
    MaxHeap() {}

    void push(k  key, v value) {
        if (!hash.find(key)) {
            Element<k,v>elem = Element<k,v>(key, value);
            arr.push_back(elem);
            int idx = arr.size() - 1;
            hash.insert(key, idx);
            heapify_up(idx);
        }

    }

    void pop_top() {
        if (arr.empty()) return;

        hash.remove(arr[0].key);

        if (arr.size() > 1) {
            arr[0] = arr.back();
            hash.insert(arr[0].key, 0);
        }
        arr.pop_back();

        if (arr.size() > 0) {
            heapify_down(0);
        }
    }

    Element<k,v> top() {
        if (arr.size() ==0) {
            throw runtime_error("empty array");
        }
        return arr[0];
    }

    vector<Element<k,v>> top_num(int num) {
        vector<Element<k,v>> res;
        num = min(num, size());
        //auto temp = arr;
        vector<Element<k,v>>temp;
        for (int i = 0; i < num; i++) {
            res.push_back(arr[0]);
            temp.push_back(arr[0]);
            pop_top();
        }
        for (int i = 0; i < num; i++) {
            push(temp.back().key, temp.back().value);
            temp.pop_back();
        }
        return res;
    }

// A very important function that is responsible for th O(1) time lookup and updating in the heaps for Recent files and biggest trees
    void update(const k& key,const v& new_value) {
        if (hash.find(key)==nullptr) {
            throw runtime_error("Key not found");
        }

        int idx = *(hash.find(key));
        v old_value = arr[idx].value;
        arr[idx].value = new_value;
//Checks where to add the newer value of the key and update it
        if (new_value > old_value) {
            heapify_up(idx);
        } else if (new_value < old_value) {
            heapify_down(idx);
        }
    }

    bool contains(const k& key) {
        return hash.find(key) != nullptr;
    }

    v get(const k& key) {
        int* idx_ptr = hash.find(key);
        if (idx_ptr == nullptr) {
            throw runtime_error("Key not found");
        }
        return arr[*idx_ptr].value;
    }

    bool empty() {
        return arr.empty();
    }
    int size() {
        return arr.size();
    }
};

//file
class File {
    public:
    TreeNode* root;
    TreeNode* active_version;
    HashMap<int, TreeNode*> version_map;
    int total_versions;
    string filename;
    time_t last_modified;

    File(const string& name) {
        filename = name;
        total_versions = 1;

        // Create root version
        root = new TreeNode(0, "", nullptr);
        root->message = "Initial Snapshot Message";
        root->is_snapshot = false;
        root->snapshot_timestamp = time(nullptr);
        active_version = root;
        version_map.insert(0, root);
        last_modified = time(nullptr);
    }

    void insert(string &content) {
        last_modified = time(nullptr);
        if (active_version->is_snapshot) {
            TreeNode* current_version = new TreeNode(total_versions, active_version->content+content, active_version);
            active_version->children.push_back(current_version);
            //parent.insert(current_version, active_version);
            active_version = current_version;
            version_map.insert(total_versions, current_version);
            total_versions++;

        }
        else {
            active_version->content += content;
        }
    }

    void update(string &content) {
        last_modified = time(nullptr);
        if (active_version->is_snapshot) {
            TreeNode* current_version = new TreeNode(total_versions, content, active_version);
            active_version->children.push_back(current_version);
            //parent.insert(current_version, active_version);
            active_version = current_version;
            version_map.insert(total_versions, current_version);
            total_versions++;
        }
        else {
            active_version->content = content;
        }
    }
    void snapshot(string &mess) {
        active_version->is_snapshot = true;
        active_version->message = mess;
        last_modified = time(nullptr);
        active_version->snapshot_timestamp = time(nullptr);
        version_map.insert(total_versions, active_version);
    }
    bool is_snapshot() {
        return active_version->is_snapshot;
    }
    bool is_valid_version(int version_id) {
        if (version_id < 0 || version_id >= total_versions) {
            return false;
        }
        TreeNode** node_ptr = version_map.find(version_id);
        return (node_ptr != nullptr && *node_ptr != nullptr);
    }
    bool rollback(int version_id = -1) {
        if (version_id == -1) {
            // Rollback to parent
            if (active_version->parent) {
                active_version = active_version->parent;
                return true;
            } else {
                return false;
            }
        } else {
            if (!is_valid_version(version_id)) {
                return false;
            }

            TreeNode** node_ptr = version_map.find(version_id);
            active_version = *node_ptr;
            return true;
        }
    }
    vector<TreeNode*> get_snapshots() {
        vector<TreeNode*> res;
        TreeNode* current = active_version;
        while (current) {
            res.push_back(current);
            current = current->parent; // traces from current to parent
        }
        return res;
    }
    TreeNode* active() {
        return active_version;
    }
};

//File System
class FileSystem {
private:
    HashMap<string,File*> files;
    MaxHeap<string,time_t>recent;
    MaxHeap<string,int>biggest;
public:
    bool file_exists(string name) {
        if (files.find(name)!=nullptr) {
            return true;
        }
        return false;
    }
//everytime any changes is made to the file the reent and biggest heaps are modified
    void create_file(string &name) {
        if (file_exists(name)) {
            return;
        }
        File* file = new File(name);
        files.insert(name, file);
        recent.push(name, file->last_modified);
        biggest.push(name, file->total_versions);
        string s = "Initial snap_message";
        snapshot_file(name,s);
    }
// All the places with (file && *file) checks if the file is a null pointer or the pointer to the file is a nullptr, in either case the file doest not exist
    string read_file(string &name) {
        File** file  = files.find(name);
        if (file && *file) {
            return (*file)->active_version->content;
        }
        return "";
    }
    void insert_in_file(string &name,string &content) {
        File** file  = files.find(name);
        if (file && *file) {
            (*file)->insert(content);
            update_recent(*file);
            update_biggest(*file);
        }
    }
    void update_file(string& name, string& content) {
        File** file = files.find(name);
        if (file && *file) {
            (*file)->update(content);
            update_recent(*file);
            update_biggest(*file);
        }
    }
    void snapshot_file(string& name, string& mess) {
        File** file = files.find(name);
        if (file && *file) {
            (*file)->snapshot(mess);
            update_recent(*file);
        }
    }
    bool rollback_file(string &filename, int version_id=-1) {
        File** file = files.find(filename);
        if (file && *file) {
            return (*file)->rollback(version_id);
        }
        return false;
    }

    string snap_message(string &name) {
        File** file = files.find(name);
        if (file && *file) {
            return (*file)->active_version->message;
        }
        return "";
    }

    bool is_snapshot(string &name) {
        File** file = files.find(name);
        if (file && *file) {
            return (*file)->is_snapshot();
        }
        return false;
    }
    int total_versions(string &name) {
        File** file = files.find(name);
        if (file && *file) {
            return (*file)->total_versions;
        }
        return -1;
    }

    vector<TreeNode*> file_history(string &filename) {
        File** file = files.find(filename);
        if (file && *file) {
            return (*file)->get_snapshots();
        }
        return vector<TreeNode*>();
    }
    TreeNode* get_active_version(string &filename) {
        File** file = files.find(filename);
        if (file && *file) {
            return (*file)->active_version;
        }
        return nullptr;
    }
    void update_recent(File* f) {
        if (recent.contains(f->filename)) {
            recent.update(f->filename, f->last_modified);
        } else {
            recent.push(f->filename, f->last_modified);
        }
    }

    void update_biggest(File* f) {
        if (biggest.contains(f->filename)) {
            biggest.update(f->filename, f->total_versions);
        } else {
            biggest.push(f->filename, f->total_versions);
        }
    }

    vector<string> recent_files(int num) {
        vector<Element<string, time_t>> num_recent = recent.top_num(num);
        vector<string> result;

        for (auto& elem : num_recent) {
            result.push_back(elem.key);
        }
        return result;
    }
    int version(string &name) {
        File** file = files.find(name);
        if (file && *file) {
            return  (*file)->total_versions;
        }
        return -1;
    }
    vector<string> biggest_trees(int num) {
        vector<Element<string, int>> num_biggest = biggest.top_num(num);
        vector<string> result;

        for (const auto& elem : num_biggest) {
            result.push_back(elem.key);
        }
        return result;
    }
};
//Function to parse messages and content with spaces
string input() {
    string s;
    cin>>ws;
    if (cin.peek() == '"') {
        cin.get();
        getline(cin, s, '"');
    } else {
        cin>>s;
    }
    return s;
}

int main() {

    FileSystem fs;
    cout << "STARTED" << endl;
    while (true) {
        string command;
        cin >> command;
        if (command == "CREATE") {
            string name;
            if (!(cin >> name)) {
                cout << "Filename missing" << endl;
                cin.clear();
                continue;
            }
            if (name.size()==0) cout<<"Please input a valid name:"<<endl;
            else {
                if (fs.file_exists(name)) {
                    cout<<"file "<<name<<" exists"<<endl;
                }
                else {
                    fs.create_file(name);
                    cout<<"file "<<name<<" created"<<endl;
                }
            }
        }
        else if (command == "READ") {
            string name;
            if (cin.peek() == '\n') {
                cout << "Name missing" << endl;
                continue;
            }
            cin>>name;
            if (name.size()==0) cout<<"Please input a valid name:"<<endl;
            else {
                if (!fs.file_exists(name)) {
                    cout<<"file "<<name<<" does not exist"<<endl;
                }
                else {

                    cout<<"file "<<name<<" content is "<<fs.read_file(name)<<endl;
                }
            }
        }
        else if (command == "INSERT") {
            string name, content;
        // checking
            if (cin.peek() == '\n') {
                cout << "Name missing" << endl;
                continue;
            }
            cin>>name;
            if (cin.peek() == '\n') {
                cout << "Content missing" << endl;
                continue;
            }
            content = input();
            if (name.size()==0) cout<<"Please input a valid name:"<<endl;
            else {
                if (!fs.file_exists(name)) {
                    cout<<"file does not exist"<<endl;
                }
                else {
                    fs.insert_in_file(name, content);
                    cout<<"file "<<name<<" content "<<content<<"is inserted"<<endl;
                    cout<<"file "<<name<<" content is "<<fs.read_file(name)<<endl;

                }
            }
        }
        else if (command == "UPDATE") {
            string name, content;
            if (cin.peek() == '\n') {
                cout << "Name missing" << endl;
                continue;
            }
            cin>>name;
            if (cin.peek() == '\n') {
                cout << "Content missing" << endl;
                continue;
            }
            content = input();
            if (name.size()==0) cout<<"Please input a valid name:"<<endl;
            else {
                if (!fs.file_exists(name)) {
                    cout<<"file "<<name<<" does not exist"<<endl;
                }
                else {
                    fs.update_file(name, content);
                    cout<<"file "<<name<<" content "<<content<<" is updated"<<endl;
                    cout<<"file "<<name<<" content is "<<fs.read_file(name)<<endl;
                }
            }
        }
        else if (command == "SNAPSHOT") {
            string name, mess;
            if (cin.peek() == '\n') {
                cout << "Name missing" << endl;
                continue;
            }
            cin>>name;
            if (cin.peek() == '\n') {
                cout << "Message missing" << endl;
                continue;
            }
            mess = input();
            if (name.size()==0) cout<<"Please input a valid name:"<<endl;
            else {
                if (!fs.file_exists(name)) {
                    cout<<"file "<<name<<" does not exist"<<endl;
                }
                else if (fs.is_snapshot(name)) {
                    cout<<"Already a snapshot"<<endl;
                }
                else {
                    fs.snapshot_file(name, mess);
                    cout<<"file "<<name<<" content "<<fs.snap_message(name)<<" is snapshotted"<<endl;
                }
            }
        }
        else if (command == "ROLLBACK") {
            string name;
            if (cin.peek() == '\n') {
                cout << "Name missing" << endl;
                continue;
            }
            cin>>name;
            int version_id = -1;
            if (cin.peek() == ' ' || cin.peek() == '\n') {
                string next;
                getline(cin, next);
                if (!next.empty()) {
                    try {
                        version_id = stoi(next);
                    } catch (...) {
                        version_id = -1;
                    }
                }
            }
            if (name.size()==0) cout<<"Please input a valid name:"<<endl;
            else {
                if (!fs.file_exists(name)) {
                    cout<<"file "<<name<<" does not exist"<<endl;
                }

                else {
                    bool done =  fs.rollback_file(name, version_id);
                    if (!done) {
                        cout<<"Invalid Version"<<endl;
                    }
                    else {
                        cout<<"File "<<name<<" Rolled Back"<<endl;
                    }
                }
            }
        }
        else if (command == "HISTORY") {
            string name;
            if (!(cin >> name)) {
                cout << "Filename missing" << endl;
                cin.clear();
                continue;
            }
            if (name.size()==0) cout<<"Please input a valid name:"<<endl;
            if (!fs.file_exists(name)) {
                cout<<"file "<<name<<" does not exist"<<endl;
            }
            else {
                vector<TreeNode*> history = fs.file_history(name);
                cout<<"Snapshot versions are"<<endl;
                for (int i = history.size()-1; i>-1; i--) {
                    TreeNode* node = history[i];
                    cout << "Version " << node->version_id<<","<< ctime(&node->snapshot_timestamp)<<","<<node->message << endl;
                    //converted timestamp to actual time
                }
            }
        }
        else if (command == "RECENT_FILES") {
            int num;
            if (!(cin >> num)) {
                cout << "Num missing" << endl;
                cin.clear();
                continue;
            }
            vector<string> recent = fs.recent_files(num);
            if (recent.empty()) {
                cout << "No files found" << endl;
            } else {
                cout << "Recent files:" << endl;
                for (const string& filename : recent) {
                    cout << filename << endl;
                }
            }
        }
        else if (command == "BIGGEST_TREES") {
            int num;
            if (!(cin >> num)) {
                cout << "Num missing" << endl;
                cin.clear();
                continue;
            }
            vector<string> bigg = fs.biggest_trees(num);
            if (bigg.empty()) {
                cout << "No files found" << endl;
            } else {
                cout << "Biggest trees:" << endl;
                for (const string& filename : bigg) {
                    string s = filename;
                    cout << filename <<' '<< fs.total_versions(s)<< endl;
                }
            }
        }
        else if (command == "EXIT") break;
        else  cout<<"Invalid command"<<endl;
    }
    return 0;
}