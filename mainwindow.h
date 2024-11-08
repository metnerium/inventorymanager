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
class MainWindow;

class AddRecordDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddRecordDialog(QWidget *parent = nullptr);
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

class EditRecordDialog : public QDialog {
    Q_OBJECT
public:
    EditRecordDialog(QSqlTableModel *model, MainWindow *parent);
private slots:
    void saveChanges();
private:
    QTableView *tableView;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    QSqlTableModel *model;
    QSqlTableModel *modelCopy;
    MainWindow* mainWindow;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    void refresh();
    QSqlTableModel *model;
private slots:
    void showDataView();
    void showAnalyticsView();
    void showAddRecordDialog();
    void showEditRecordDialog();
private:
    void setupUI();
    void createAddRecordDialog();
    QSqlDatabase db;
    QSqlTableModel *modelCopy;
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *analyticsButton;
    AddRecordDialog *addRecordDialog;
};

#endif // MAINWINDOW_H
