#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>

class QVBoxLayout;
class QTableView;
class QPushButton;
class QLabel;

class AddRecordDialog : public QDialog {
    Q_OBJECT

public:
    AddRecordDialog(QWidget *parent = nullptr);

private:
    QLineEdit *storeNameEdit;
    QLineEdit *productArticleEdit;
    QLineEdit *productNameEdit;
    QDateEdit *receiptDateEdit;
    QLineEdit *amountEdit;
    QLineEdit *priceEdit;
    QLineEdit *soldCountEdit;
    QLineEdit *employeeNameEdit;
    QPushButton *addButton;
    QPushButton *cancelButton;

    friend class MainWindow;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void showDataView();
    void showPrintView();
    void showAnalyticsView();
    void showAddRecordDialog();

private:
    void setupUI();
    void createAddRecordDialog();

    QSqlDatabase db;
    QSqlTableModel *model;

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QTableView *tableView;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *printButton;
    QPushButton *analyticsButton;

    AddRecordDialog *addRecordDialog;
};

#endif // MAINWINDOW_H
