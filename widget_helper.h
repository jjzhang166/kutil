#ifndef _KUTIL_WIDGET_HELPER_H_
#define _KUTIL_WIDGET_HELPER_H_

#include "macro.h"
#include <QObject>
#include <QString>
#include <QUuid>
#include <QByteArray>
#include <QVariant>
#include <QDebug>
#include <QTextCodec>

class QWidget;
class QAbstractItemModel;
class QStandardItemModel;

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QHeaderView>
#include <QTableWidget>

namespace kutil
{
    namespace widget {

        // ���ڵ�����λ��
        QRect windowRect(QWidget *w);

        QPoint GetWidgetScreenPos(QWidget* widget);

        /**
        *  ΪwidgetӦ����ʽ��
        *
        *	@return
        *		true    �ɹ�
        *	@param
        *		-[in]
        *      wdt   ��ҪӦ����ʽ��Ŀؼ�
        *      res   ��Դ·�����������ⲿ��Դ���߳����ڽ���Դ
        *
        **/
        bool ApplyStyleSheet(QWidget* wdt, const QString& res);

        // ��ȡ��Դ
        QIcon GetResourceIcon(const QString& themeName, const QString& resouce_name);

        // �ؼ����е�ptλ��
        void CenterWidget(QWidget* w, QPoint pt);

        // �ؼ����е���Ļ����
        void CenterWidgetDesktop(QWidget* w);

        // ���ǵ���Ļ�ϣ�������������
        void MoveToFullWorkArea(QWidget* wdt);

        // ����model�ڵ�����ΪCSV��Execel���
        void SaveToCSV(QStandardItemModel* m);

        //����QTableWidget�ڵ�����ΪCSV
        void SaveToCSV(QTableWidget*);

        //����QList<QStringList>�ڵ�����ΪCSV
        bool SaveToCSV(QList<QStringList>&);

        QList<QStringList> ReadFromCVS(QTextCodec *codec = nullptr);
        bool ReadFromCVS(QList<QStringList>& ctn, QTextCodec *codec = nullptr);

        // ����model�ڵ����ݵ������壬
        void CopyTableModel2Clipboard(QStandardItemModel* m);

        // ����QTableWidget�����а�
        void CopyTableWidget2Clipboard(QTableWidget*);

        //�ж�float�Ƿ�Ϊ0
        bool IsFloatZero(const float&);

        //�ж�double�Ƿ�Ϊ0
        bool IsDoubleZero(const double&);

        //��ʾ��ӯ��ʾ�� ���� ��ֵ �ǵ���
        //void ShowProfitLoss(const QString&, const QString&,const QString&,
        //bool bNewPos = false);

        //��ʾƽ�ֲ����Ľ���
        void ShowClosePosView(const QString&, const QString&);

        // ��ձ�
        void ClearTable(QAbstractItemModel* m);

        // �жϿؼ�w��ǰ�Ƿ��ھ��н���Ĵ����У����߱�����Ǿ������뽹��Ŀؼ���
        bool IsInFocusingWindow(QWidget* w);

        // ��ȡItemView��ǰ�ɼ��ĵ�һ�У������һ�е�����
        bool indexVisibleItem(QAbstractItemView* v, __p_out QModelIndex& idxFirst, __p_out QModelIndex& idxLast);
    }

    namespace widget {

        struct  TBLFiled
        {
            TBLFiled(const QString& title, const QString& data_filed, int width, int precision,
                QHeaderView::ResizeMode resize_m = QHeaderView::Interactive, QColor bk = QColor())
                :title_(title), data_filed_(data_filed), precision_(precision),
                resize_mode_(resize_m), width_(width), bk_(bk)
            {
            }

            QString     title_;
            // ��Ӧ�Ľṹ������
            QString     data_filed_;
            QHeaderView::ResizeMode resize_mode_ = QHeaderView::Interactive;
            int         width_ = 0;
            int         precision_ = -1;    // ��Ϊdouble��ʱ�����þ���
            QColor      bk_;    // ����ɫ
        };

        class KTableHelper
        {
        public:
            /*
            *	������ѡ�е�item(һ��������ܰ������item)���е��� f
            *      ��� f ����false����ִֹͣ��
            */
            template<typename TblType>
            static void ForeachSelectItem(TblType* v,
                std::function<bool(const QModelIndex& idx)> f)
            {
                QItemSelectionModel * m = v->selectionModel();
                for (auto &idx : m->selectedIndexes())
                {
                    if (!f(idx))
                    {
                        break;
                    }
                }
            }

            /*
            *	������ѡ�е��н��е��� f
            *      ��� f ����false����ִֹͣ��
            */
            template<typename TblType>
            static void ForeachSelectRows(TblType* v,
                std::function<bool(const QModelIndex& idx)> f)
            {
                QItemSelectionModel * m = v->selectionModel();
                for (auto &idx : m->selectedRows())
                {
                    if (!f(idx))
                    {
                        break;
                    }
                }
            }

            /*
            *	��ȡ��idx��ͬһ�е� �� col �е�index
            */
            static QModelIndex ModelIndexOfCol(QAbstractItemModel *m,
                const QModelIndex& idx, int col)
            {
                if ((nullptr == m)
                    || !idx.isValid()
                    || (m->columnCount() <= col))
                {
                    Q_ASSERT(false);
                    return QModelIndex();
                }

                if (idx.column() == col)
                    return idx;

                return m->index(idx.row(), col, idx.parent());
            }

            template<typename TblType>
            static QVariant ItemData(TblType* v, const QModelIndex& idx,
                int col, int role = Qt::DisplayRole)
            {
                QAbstractItemModel* m = v->model();
                if (nullptr != m)
                {
                    QModelIndex i = ModelIndexOfCol(m, idx, col);
                    if (i.isValid())
                    {
                        return m->data(i, role);
                    }
                }
                return QVariant();
            }

            template < typename TblType >
            static void ClearTable(TblType * v)
            {
                QAbstractItemModel *m = v->model();
                if (nullptr != m)
                {
                    m->removeRows(0, m->rowCount());
                }
            }

            static void InitTableWidget(QTableWidget* v, QList<TBLFiled> fields,
                const QString& unique_col, QSize min_size)
            {
                // Ĭ�Ͻ�ֹ�ֶ��༭
                v->setEditTriggers(QAbstractItemView::NoEditTriggers);
                v->setColumnCount(fields.size());

                int i = 0;
                for (const TBLFiled& f : fields)
                {
                    v->horizontalHeader()->setSectionResizeMode(i, f.resize_mode_);
                    v->setHorizontalHeaderItem(i, new QTableWidgetItem(f.title_));
                    if (f.width_ != -1)
                    {
                        // v->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
                        v->setColumnWidth(i, f.width_);
                    }
                    // v->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
                    ++i;
                }

                v->setMinimumSize(min_size);
                // v->resizeColumnsToContents();

                // ��¼����
                table_fields_.insert(v, fields);
                table_unique_col_.insert(v, unique_col);
            }

#ifdef QVIRTUALTABLEWIDGET_H
            static void InitTableWidget(QVirtualTableWidget* v, QList<TBLFiled> fields,
                const QString& unique_col, QSize min_size)
            {
                Q_ASSERT(nullptr != v->model());

                // Ĭ�Ͻ�ֹ�ֶ��༭
                v->setEditTriggers(QAbstractItemView::NoEditTriggers);
                v->setColumnCount(fields.size());

                int col = 0;
                for (const TBLFiled& tf : fields)
                {
                    v->setHorizontalHeaderItem(col, tf.title_);
                    // v->horizontalHeader()->setSectionResizeMode(col, tf.resize_mode_);
                    // v->setHorizontalHeaderItem(col, new QTableWidgetItem(tf.title_));
                    if (tf.bk_.isValid())
                    {
                        v->setColumnBkColor(col, tf.bk_);
                    }

                    if (tf.width_ != -1)
                    {
                        // v->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Interactive);
                        v->setColumnWidth(col, tf.width_);
                    }
                    // v->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
                    ++col;
                }

                // ���ص���ֱ�ı�ͷ
                v->verticalHeader()->hide();

                // ѡ���ʱ��
                v->setSelectionMode(QAbstractItemView::SingleSelection);
                v->setSelectionBehavior(QAbstractItemView::SelectRows);

                // ˢ�£�������Ŀǰ����֪����ô���£���ˢ�¾Ͳ�����ʾ��ͷ
                v->setShowFixedRows(false);
                v->setShowFixedRows(true);
                //         v->setShowFilter(true);
                //         v->setShowFilter(false);
                v->setMinimumSize(min_size);

                col = 0;
                for (const TBLFiled& tf : fields)
                {
                    if (tf.width_ != -1)
                    {
                        v->setColumnWidth(col, tf.width_);
                    }

                    if (tf.precision_ != -1)
                    {
                        v->setColumnDoublePrecision(col, tf.precision_);
                    }
                    ++col;
                }
                v->setShowFilter(true);
                v->setShowFilter(false);
                // v->resizeColumnsToContents();

                // ��¼����
                table_fields_.insert(v, fields);
                table_unique_col_.insert(v, unique_col);
            }
#endif

            static QList<TBLFiled> Fileds(const QWidget* v)
            {
                return table_fields_.value(v);
            }

#define CODE_ID_FILED QStringLiteral("#code")
#define INVALID_INDEX (-1)

            template<typename _Tbl>
            static int FindCol(_Tbl* tbl, const QString& filed)
            {
                int col = 0;
                for (const TBLFiled& t : KTableHelper::Fileds(tbl))
                {
                    if (filed == t.data_filed_)
                    {
                        return col; // �ҵ���
                    }
                    col++;
                }
                return INVALID_INDEX;
            }

            static int UniqueCol(const QWidget* v)
            {
                return FindCol(v, table_unique_col_.value(v));
            }

            template<typename _Tbl>
            static int FindRow(_Tbl* t, const QString& s_code)
            {
                int code_filed = KTableHelper::UniqueCol(t);
                if (INVALID_INDEX == code_filed)
                {
                    return INVALID_INDEX;
                }

                int rows = t->rowCount();
                for (int i = 0; i < rows; ++i)
                {
                    if (t->getCellText(i, code_filed) == s_code)
                    {
                        return i;
                    }
                }
                return -1;
            }

        private:
            static QHash<const QWidget*, QList<TBLFiled>> table_fields_;
            // Ψһ������
            static QHash<const QWidget*, QString> table_unique_col_;
        };
    }

};

#endif // _KUTIL_WIDGET_HELPER_H_
