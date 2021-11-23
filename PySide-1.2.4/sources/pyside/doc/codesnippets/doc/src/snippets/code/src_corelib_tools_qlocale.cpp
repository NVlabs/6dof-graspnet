//! [0]
egyptian = QLocale(QLocale.Arabic, QLocale.Egypt)
s1 = egyptian.toString(1.571429E+07, 'e')
s2 = egyptian.toString(10)

(d, ok) = egyptian.toDouble(s1)
(i, ok) = egyptian.toInt(s2)
//! [0]


//! [1]
QLocale.setDefault(QLocale(QLocale.Hebrew, QLocale.Israel))
hebrew = QLocale() # Constructs a default QLocale
s1 = hebrew.toString(15714.3, 'e')

QLocale.setDefault(QLocale(QLocale.C))
c = QLocale()
(d, ok) = c.toDouble("1234,56")   # ok == false
(d, ok) = c.toDouble("1234.56")   # ok == true, d == 1234.56

QLocale.setDefault(QLocale(QLocale.German))
german = QLocale()
(d, ok) = german.toDouble("1234,56")   # ok == true, d == 1234.56
(d, ok) = german.toDouble("1234.56")   # ok == true, d == 1234.56

QLocale.setDefault(QLocale(QLocale.English, QLocale.UnitedStates))
english = QLocale()
string = '%s %s %10x' % (12345, english.toString(12345), 12345)
# string == "12345 12,345 3039"
//! [1]


//! [2]
korean = QLocale("ko")
swiss = QLocale("de_CH")
//! [2]


//! [3]
c = QLocale(QLocale.C)
(d, ok) = c.toDouble( "1234.56" )  # ok == true, d == 1234.56
(d, ok) = c.toDouble( "1,234.56" ) # ok == true, d == 1234.56
(d, ok) = c.toDouble( "1234,56" )  # ok == false

german = QLocale(QLocale.German)
(d, ok) = german.toDouble( "1234,56" )  # ok == true, d == 1234.56
(d, ok) = german.toDouble( "1.234,56" ) # ok == true, d == 1234.56
(d, ok) = german.toDouble( "1234.56" )  # ok == false

(d, ok) = german.toDouble( "1.234" )    # ok == true, d == 1234.0
//! [3]
