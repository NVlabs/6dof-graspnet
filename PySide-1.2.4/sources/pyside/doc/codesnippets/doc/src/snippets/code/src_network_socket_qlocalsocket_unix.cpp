//! [0]
socket.connectToServer("market")
if (socket.waitForConnected(1000))
    print("Connected!")
//! [0]


//! [1]
socket.disconnectFromServer()
if (socket.waitForDisconnected(1000))
    print("Disconnected!")
//! [1]
