import QtQuick 2.0
import QtQuick.Controls 2.1
import QtWebView 1.1

Page {
    property alias webView: webView
    WebView {
        id: webView
        url: "http://localhost:3000/"
        anchors.fill: parent
    }
}
