#include "mainwindow.h"

#include <QVBoxLayout>
#include <QTableView>
#include <QPushButton>
#include <QPrintDialog>
#include <QCoreApplication>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextDocument>
#include <QPrinter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    // Настройка соединения с базой данных SQLite
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Failed to open database");
        return;
    }

    // Создание таблицы в базе данных
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS products ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "store_name TEXT, "
               "product_article TEXT, "
               "product_name TEXT, "
               "receipt_date DATE, "
               "amount INTEGER, "
               "price REAL, "
               "sold_count INTEGER, "
               "employee_name TEXT)");

    // Создание модели данных
    model = new QSqlTableModel(this, db);
    model->setTable("products");
    model->select();

    setupUI();
    createAddRecordDialog();
}

void MainWindow::setupUI() {
    centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(centralWidget);
    auto *viewButton = new QPushButton("Просмотр данных");
    auto *exitbutton = new QPushButton("Выход");
    printButton = new QPushButton("Печать");
    analyticsButton = new QPushButton("Аналитика");
    mainLayout->addWidget(viewButton);
    mainLayout->addWidget(printButton);
    mainLayout->addWidget(analyticsButton);
    mainLayout->addWidget(exitbutton);
    connect(viewButton, &QPushButton::clicked, this, &MainWindow::showDataView);
    connect(analyticsButton, &QPushButton::clicked, this, &MainWindow::showAnalyticsView);
    connect(printButton, &QPushButton::clicked, this, &MainWindow::showPrintView);
    connect(exitbutton, &QPushButton::clicked, this, &MainWindow::close);

    setCentralWidget(centralWidget);
}
AddRecordDialog::AddRecordDialog(QWidget *parent)
    : QDialog(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->resize(400, 500);
    QLabel *storeNameLabel = new QLabel("Store Name:");
    storeNameEdit = new QLineEdit;
    layout->addWidget(storeNameLabel);
    layout->addWidget(storeNameEdit);

    QLabel *productArticleLabel = new QLabel("Product Article:");
    productArticleEdit = new QLineEdit;
    layout->addWidget(productArticleLabel);
    layout->addWidget(productArticleEdit);

    QLabel *productNameLabel = new QLabel("Product Name:");
    productNameEdit = new QLineEdit;
    layout->addWidget(productNameLabel);
    layout->addWidget(productNameEdit);

    QLabel *receiptDateLabel = new QLabel("Receipt Date:");
    receiptDateEdit = new QDateEdit(QDate::currentDate());
    receiptDateEdit->setCalendarPopup(true);
    layout->addWidget(receiptDateLabel);
    layout->addWidget(receiptDateEdit);

    QLabel *amountLabel = new QLabel("Amount:");
    amountEdit = new QLineEdit;
    layout->addWidget(amountLabel);
    layout->addWidget(amountEdit);

    QLabel *priceLabel = new QLabel("Price:");
    priceEdit = new QLineEdit;
    layout->addWidget(priceLabel);
    layout->addWidget(priceEdit);

    QLabel *soldCountLabel = new QLabel("Sold Count:");
    soldCountEdit = new QLineEdit;
    layout->addWidget(soldCountLabel);
    layout->addWidget(soldCountEdit);

    QLabel *employeeNameLabel = new QLabel("Employee Name:");
    employeeNameEdit = new QLineEdit;
    layout->addWidget(employeeNameLabel);
    layout->addWidget(employeeNameEdit);

    addButton = new QPushButton("Добавить");
    cancelButton = new QPushButton("Назад");
    layout->addWidget(addButton);
    layout->addWidget(cancelButton);
}
void MainWindow::createAddRecordDialog() {
    addRecordDialog = new AddRecordDialog(this);

    connect(addRecordDialog->addButton, &QPushButton::clicked, [=]() {
        QSqlRecord record = model->record();
        record.setValue("store_name", addRecordDialog->storeNameEdit->text());
        record.setValue("product_article", addRecordDialog->productArticleEdit->text());
        record.setValue("product_name", addRecordDialog->productNameEdit->text());
        record.setValue("receipt_date", addRecordDialog->receiptDateEdit->date());
        record.setValue("amount", addRecordDialog->amountEdit->text().toInt());
        record.setValue("price", addRecordDialog->priceEdit->text().toDouble());
        record.setValue("sold_count", addRecordDialog->soldCountEdit->text().toInt());
        record.setValue("employee_name", addRecordDialog->employeeNameEdit->text());

        if (model->insertRecord(-1, record)) {
            model->select();
        } else {
            QMessageBox::critical(this, "Error", "Failed to add record");
        }

        addRecordDialog->close();
    });

    connect(addRecordDialog->cancelButton, &QPushButton::clicked, addRecordDialog, &QDialog::close);
}

void MainWindow::showDataView() {
    QDialog *dataView = new QDialog(this);
    dataView->setWindowTitle("Просмотр данных");
    dataView->resize(800, 600);
    QVBoxLayout *layout = new QVBoxLayout(dataView);

    QTableView *tableView = new QTableView(dataView);
    tableView->setModel(model);
    layout->addWidget(tableView);

    QPushButton *addButton = new QPushButton("Новая запись");
    QPushButton *editButton = new QPushButton("Редактировать");
    QPushButton *backButton = new QPushButton("Назад");

    layout->addWidget(addButton);
    layout->addWidget(editButton);
    layout->addWidget(backButton);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::showAddRecordDialog);
    connect(editButton, &QPushButton::clicked, [=]() {
        tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    });
    connect(backButton, &QPushButton::clicked, dataView, &QDialog::close);

    dataView->exec();
}

void MainWindow::showPrintView() {
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::NativeFormat);

    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QString html;
        html += "<html><head><style>table, th, td { border: 1px solid black; border-collapse: collapse; }</style></head><body><table>";

        // Добавляем заголовки столбцов
        html += "<tr>";
        QSqlRecord record = model->record();
        for (int i = 0; i < record.count(); ++i) {
            html += "<th>" + record.fieldName(i) + "</th>";
        }
        html += "</tr>";

        // Получаем все записи из модели и добавляем их в HTML-таблицу
        QSqlQuery query("SELECT * FROM products");
        while (query.next()) {
            html += "<tr>";
            for (int i = 0; i < query.record().count(); ++i) {
                html += "<td>" + query.value(i).toString() + "</td>";
            }
            html += "</tr>";
        }

        html += "</table></body></html>";

        // Создаем QTextDocument и устанавливаем HTML-данные для печати
        QTextDocument document;
        document.setHtml(html);

        // Выполняем печать
        document.print(&printer);
    }
}

void MainWindow::showAnalyticsView() {
    QDialog *analyticsView = new QDialog(this);
    analyticsView->setWindowTitle("Аналитика");
    analyticsView->resize(600, 400);
    QVBoxLayout *layout = new QVBoxLayout(analyticsView);

    QLabel *totalRecordsLabel = new QLabel("Total Records: " + QString::number(model->rowCount()));
    layout->addWidget(totalRecordsLabel);

    QSqlQueryModel *storeAnalyticsModel = new QSqlQueryModel(analyticsView);
    storeAnalyticsModel->setQuery("SELECT store_name, COUNT(*) AS total_records, SUM(amount) AS total_amount, SUM(price * amount) AS total_cost FROM products GROUP BY store_name");

    QLabel *totalStoresLabel = new QLabel("Total Stores: " + QString::number(storeAnalyticsModel->rowCount()));
    layout->addWidget(totalStoresLabel);

    QTableView *storeAnalyticsView = new QTableView(analyticsView);
    storeAnalyticsView->setModel(storeAnalyticsModel);
    layout->addWidget(storeAnalyticsView);

    QPushButton *backButton = new QPushButton("Back");
    layout->addWidget(backButton);

    connect(backButton, &QPushButton::clicked, analyticsView, &QDialog::close);

    analyticsView->exec();
}

void MainWindow::showAddRecordDialog() {
    addRecordDialog->setModal(true);
    addRecordDialog->show();
}
