#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    // Настройка соединения с базой данных SQLite
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных");
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
    modelCopy = new QSqlTableModel(this, db);
    modelCopy->setTable("products");
    modelCopy->select();
    setupUI();
    createAddRecordDialog();
}

void MainWindow::setupUI() {
    centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(centralWidget);
    auto *viewButton = new QPushButton("Просмотр данных");
    auto *exitButton = new QPushButton("Выход");
    analyticsButton = new QPushButton("Аналитика");
    mainLayout->addWidget(viewButton);
    mainLayout->addWidget(analyticsButton);
    mainLayout->addWidget(exitButton);
    connect(viewButton, &QPushButton::clicked, this, &MainWindow::showDataView);
    connect(analyticsButton, &QPushButton::clicked, this, &MainWindow::showAnalyticsView);
    connect(exitButton, &QPushButton::clicked, this, &MainWindow::close);

    setCentralWidget(centralWidget);
}

AddRecordDialog::AddRecordDialog(QWidget *parent)
    : QDialog(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->resize(400, 500);
    QLabel *storeNameLabel = new QLabel("Название магазина:");
    storeNameEdit = new QLineEdit;
    layout->addWidget(storeNameLabel);
    layout->addWidget(storeNameEdit);

    QLabel *productArticleLabel = new QLabel("Артикул товара:");
    productArticleEdit = new QLineEdit;
    layout->addWidget(productArticleLabel);
    layout->addWidget(productArticleEdit);

    QLabel *productNameLabel = new QLabel("Название товара:");
    productNameEdit = new QLineEdit;
    layout->addWidget(productNameLabel);
    layout->addWidget(productNameEdit);

    QLabel *receiptDateLabel = new QLabel("Дата поступления:");
    receiptDateEdit = new QDateEdit(QDate::currentDate());
    receiptDateEdit->setCalendarPopup(true);
    layout->addWidget(receiptDateLabel);
    layout->addWidget(receiptDateEdit);

    QLabel *amountLabel = new QLabel("Количество:");
    amountEdit = new QLineEdit;
    layout->addWidget(amountLabel);
    layout->addWidget(amountEdit);

    QLabel *priceLabel = new QLabel("Цена:");
    priceEdit = new QLineEdit;
    layout->addWidget(priceLabel);
    layout->addWidget(priceEdit);

    QLabel *soldCountLabel = new QLabel("Количество проданных:");
    soldCountEdit = new QLineEdit;
    layout->addWidget(soldCountLabel);
    layout->addWidget(soldCountEdit);

    QLabel *employeeNameLabel = new QLabel("Имя сотрудника:");
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
            QMessageBox::critical(this, "Ошибка", "Не удалось добавить запись");
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
    QPushButton *refreshButton = new QPushButton("Обновить таблицу");
    QPushButton *backButton = new QPushButton("Назад");

    layout->addWidget(addButton);
    layout->addWidget(refreshButton);
    layout->addWidget(editButton);
    layout->addWidget(backButton);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::showAddRecordDialog);
    connect(refreshButton, &QPushButton::clicked, this, [=](){
        tableView->setModel(model);
    });
    connect(editButton, &QPushButton::clicked, this, &MainWindow::showEditRecordDialog);
    connect(backButton, &QPushButton::clicked, dataView, &QDialog::close);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dataView->exec();
}

void MainWindow::showAnalyticsView() {
    QDialog *analyticsView = new QDialog(this);
    analyticsView->setWindowTitle("Аналитика");
    analyticsView->resize(600, 400);
    QVBoxLayout *layout = new QVBoxLayout(analyticsView);

    QLabel *totalRecordsLabel = new QLabel("Всего записей: " + QString::number(model->rowCount()));
    layout->addWidget(totalRecordsLabel);

    QSqlQueryModel *storeAnalyticsModel = new QSqlQueryModel(analyticsView);
    storeAnalyticsModel->setQuery("SELECT store_name AS 'Магазин', "
                                  "COUNT(*) AS 'Всего записей', "
                                  "SUM(amount) AS 'Общее количество', "
                                  "SUM(price * amount) AS 'Общая стоимость' "
                                  "FROM products GROUP BY store_name");

    QLabel *totalStoresLabel = new QLabel("Всего магазинов: " + QString::number(storeAnalyticsModel->rowCount()));
    layout->addWidget(totalStoresLabel);

    QTableView *storeAnalyticsView = new QTableView(analyticsView);
    storeAnalyticsView->setModel(storeAnalyticsModel);
    layout->addWidget(storeAnalyticsView);

    QPushButton *backButton = new QPushButton("Назад");
    layout->addWidget(backButton);

    connect(backButton, &QPushButton::clicked, analyticsView, &QDialog::close);

    analyticsView->exec();
}

EditRecordDialog::EditRecordDialog(QSqlTableModel* model, MainWindow* parent)
    : QDialog(parent), model(model), mainWindow(parent)
{
    setWindowTitle("Редактирование записей");
    resize(800, 600);

    QVBoxLayout* layout = new QVBoxLayout(this);

    tableView = new QTableView(this);
    tableView->setModel(model);
    layout->addWidget(tableView);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    saveButton = new QPushButton("Сохранить");
    cancelButton = new QPushButton("Отмена");
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, &EditRecordDialog::saveChanges);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
}

void EditRecordDialog::saveChanges()
{
    if (model->submitAll()) {
        mainWindow->refresh();
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить изменения");
    }
}

void MainWindow::showAddRecordDialog() {
    addRecordDialog->storeNameEdit->clear();
    addRecordDialog->productArticleEdit->clear();
    addRecordDialog->productNameEdit->clear();
    addRecordDialog->receiptDateEdit->setDate(QDate::currentDate());
    addRecordDialog->amountEdit->clear();
    addRecordDialog->priceEdit->clear();
    addRecordDialog->soldCountEdit->clear();
    addRecordDialog->employeeNameEdit->clear();
    addRecordDialog->setModal(true);
    addRecordDialog->show();
}

void MainWindow::showEditRecordDialog()
{
    delete modelCopy;
    modelCopy = new QSqlTableModel(this, db);
    modelCopy->setTable("products");
    modelCopy->select();
    EditRecordDialog editDialog(modelCopy, this);
    if (editDialog.exec() == QDialog::Accepted) {
        model = modelCopy;
        modelCopy = nullptr;
    }
}

void MainWindow::refresh() {
    model->select();
}
