//! [0]
QHash<QString, int> hash;
//! [0]


//! [1]
hash["one"] = 1;
hash["three"] = 3;
hash["seven"] = 7;
//! [1]


//! [2]
hash.insert("twelve", 12);
//! [2]


//! [3]
int num1 = hash["thirteen"];
int num2 = hash.value("thirteen");
//! [3]


//! [4]
int timeout = 30;
if (hash.contains("TIMEOUT"))
    timeout = hash.value("TIMEOUT");
//! [4]


//! [5]
int timeout = hash.value("TIMEOUT", 30);
//! [5]


//! [6]
// WRONG
QHash<int, QWidget *> hash;
...
for (int i = 0; i < 1000; ++i) {
    if (hash[i] == okButton)
        cout << "Found button at index " << i << endl;
}
//! [6]


//! [7]
QHashIterator<QString, int> i(hash);
while (i.hasNext()) {
    i.next();
    cout << i.key() << ": " << i.value() << endl;
}
//! [7]


//! [8]
QHash<QString, int>::const_iterator i = hash.constBegin();
while (i != hash.constEnd()) {
    cout << i.key() << ": " << i.value() << endl;
    ++i;
}
//! [8]


//! [9]
hash.insert("plenty", 100);
hash.insert("plenty", 2000);
// hash.value("plenty") == 2000
//! [9]


//! [10]
QList<int> values = hash.values("plenty");
for (int i = 0; i < values.size(); ++i)
    cout << values.at(i) << endl;
//! [10]


//! [11]
QHash<QString, int>::iterator i = hash.find("plenty");
while (i != hash.end() && i.key() == "plenty") {
    cout << i.value() << endl;
    ++i;
}
//! [11]


//! [12]
QHash<QString, int> hash;
...
foreach (int value, hash)
    cout << value << endl;
//! [12]


//! [13]
#ifndef EMPLOYEE_H
#define EMPLOYEE_H

class Employee
{
public:
    Employee() {}
    Employee(const QString &name, const QDate &dateOfBirth);
    ...

private:
    QString myName;
    QDate myDateOfBirth;
};

inline bool operator==(const Employee &e1, const Employee &e2)
{
    return e1.name() == e2.name()
           && e1.dateOfBirth() == e2.dateOfBirth();
}

inline uint qHash(const Employee &key)
{
    return qHash(key.name()) ^ key.dateOfBirth().day();
}

#endif // EMPLOYEE_H
//! [13]


//! [14]
QHash<QString, int> hash;
hash.reserve(20000);
for (int i = 0; i < 20000; ++i)
    hash.insert(keys[i], values[i]);
//! [14]


//! [15]
QHash<QObject *, int> objectHash;
...
QHash<QObject *, int>::iterator i = objectHash.find(obj);
while (i != objectHash.end() && i.key() == obj) {
    if (i.value() == 0) {
        i = objectHash.erase(i);
    } else {
        ++i;
    }
}
//! [15]


//! [16]
QHash<QString, int> hash;
...
QHash<QString, int>::const_iterator i = hash.find("HDR");
while (i != hash.end() && i.key() == "HDR") {
    cout << i.value() << endl;
    ++i;
}
//! [16]


//! [17]
QHash<QString, int> hash;
hash.insert("January", 1);
hash.insert("February", 2);
...
hash.insert("December", 12);

QHash<QString, int>::iterator i;
for (i = hash.begin(); i != hash.end(); ++i)
    cout << i.key() << ": " << i.value() << endl;
//! [17]


//! [18]
QHash<QString, int>::iterator i;
for (i = hash.begin(); i != hash.end(); ++i)
    i.value() += 2;
//! [18]


//! [19]
QHash<QString, int>::iterator i = hash.begin();
while (i != hash.end()) {
    if (i.key().startsWith("_"))
        i = hash.erase(i);
    else
        ++i;
}
//! [19]


//! [20]
QHash<QString, int>::iterator i = hash.begin();
while (i != hash.end()) {
    QHash<QString, int>::iterator prev = i;
    ++i;
    if (prev.key().startsWith("_"))
        hash.erase(prev);
}
//! [20]


//! [21]
// WRONG
while (i != hash.end()) {
    if (i.key().startsWith("_"))
        hash.erase(i);
    ++i;
}
//! [21]


//! [22]
if (i.key() == "Hello")
    i.value() = "Bonjour";
//! [22]


//! [23]
QHash<QString, int> hash;
hash.insert("January", 1);
hash.insert("February", 2);
...
hash.insert("December", 12);

QHash<QString, int>::const_iterator i;
for (i = hash.constBegin(); i != hash.constEnd(); ++i)
    cout << i.key() << ": " << i.value() << endl;
//! [23]


//! [24]
QMultiHash<QString, int> hash1, hash2, hash3;

hash1.insert("plenty", 100);
hash1.insert("plenty", 2000);
// hash1.size() == 2

hash2.insert("plenty", 5000);
// hash2.size() == 1

hash3 = hash1 + hash2;
// hash3.size() == 3
//! [24]


//! [25]
QList<int> values = hash.values("plenty");
for (int i = 0; i < values.size(); ++i)
    cout << values.at(i) << endl;
//! [25]


//! [26]
QMultiHash<QString, int>::iterator i = hash.find("plenty");
while (i != hash.end() && i.key() == "plenty") {
    cout << i.value() << endl;
    ++i;
}
//! [26]
