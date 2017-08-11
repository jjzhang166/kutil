#ifndef _KUTIL_MISC_H_
#define _KUTIL_MISC_H_

#include "macro.h"
#include <QObject>
#include <QString>
#include <QUuid>
#include <QByteArray>
#include <QVariant>
#include <QDebug>
#include <QTextCodec>

namespace kutil
{

    // Ψһ�ַ���
    inline QString UuidString()
    {
        // ȥ�� ��������
        return QUuid::createUuid().toString().remove('{').remove('}');
    }

    // ��ʱ�����ɿɶ��ı�ţ������ڵ��ü����1�����ϵ�ʹ��
    /*
     *	ע�⣡����
     *      katie����ʹ�� KatieReadableUniqueString����Ψһ���
     *      ��Ҫʹ��Ĭ�ϲ�������������ǹ�����Ƶ���ô���õ�
     *      ��Ϊ������ֱ�ӵ�����������������ڸ��ԵĿ����ڴ˻�����ʵ���Լ��ĺ���
     */
    QString ReadableUniqueString(const QString& prefix, const QString& postfix /*= ""*/);

    // ͳ���ļ����µ��ļ�����
    void ScanFiles(const QString& dir, const QStringList& filter,
        bool recursive, std::function<bool(const QString& url)> cb);

    /**
    *	д�ı��ļ�
    *      ������ ֧�ָ�ʽ����
    *
    **/
    enum EnumWriteTextMode {
        WriteAppend,
        WritePreAppend,
        OverWrite,
    };
    bool WriteTextFile(const QString& file_path, const QString& content,
        EnumWriteTextMode mode = EnumWriteTextMode::OverWrite, QTextCodec* codec = QTextCodec::codecForName("UTF-8"));

    // һ���Զ�ȡ���е�����
    QString ReadTextFile(const QString& file_path, QTextCodec* codec = nullptr);

    // ��ȡ��Դ
    QString GetResourceFileName(const QString& themeName, const QString& resouce_name);

    // �źŲ������Ƿ�ɹ�
    inline bool CheckedConnect(QObject* sender, const char* signal,
        QObject* reciver, const char* slot, Qt::ConnectionType typ = Qt::AutoConnection)
    {
        Q_ASSERT(nullptr != sender);
        Q_ASSERT(nullptr != reciver);
        Q_ASSERT(nullptr != slot);
        Q_ASSERT(nullptr != signal);

        QMetaObject::Connection cn =
            sender->connect(sender, signal, reciver, slot, typ);
        if (!cn)
        {
            qDebug() << "Signal:" << signal << "\n"
                << "Slot:" << slot;

            Q_ASSERT(false);
            return false;
        }
        return true;
    }

    // ȥ�����n���ַ�
    inline QString RemoveLast(const QString &s, int n)
    {
        if (s.length() <= n)
            return "";
        QString ret = s;
        return ret.remove(s.length() - n, n);
    }

    // ��ǰ����
    QString CurrentDate();

    // QVariant�����л�
    QByteArray SaveVariant(const QVariant& v);
    QVariant LoadVariant(const QByteArray& v);

    template <typename _Cont, typename T>
    const T& PrevOrNext(const _Cont& c, const T& current, bool next,
        std::function<bool(const T&)> isFine = [](const T&)->bool { return true;  })
    {
        static T DEF_VAL;

        for (auto i = c.begin(); i != c.end(); ++i)
        {
            if (!isFine(*i))
                continue;

            if (*i == current)
            {
                if (!next)
                {
                    // ��һ��
                    return (i == c.begin()) ? c.last() : *(--i);
                }
                else
                {
                    // ��һ��
                    ++i;
                    return (i == c.end()) ? c.first() : *i;
                }
                break;
            }
        }

        if (c.size() > 0)
        {
            // Ĭ�Ϸ��ص�һ��
            return c.front();
        }

        // ����ǿ�����/����δ�ҵ�
        Q_ASSERT(false);
        return DEF_VAL;
    }

    // ������ľ�ȷ��С�������λ��С�����ȥ��С����
    // 12.30  -> 12  (ȥ��С����)
    // 123000  ->  12.30 ��
    // 1,2300,0000  ->  1.23 ��
    QString ReadableNum(double f);

    inline QString normalFilename(const QString& f) {
        QString name = f;
        return name.replace(QRegExp("[/*?:<>|=&;\"\\\\]"), "")
            .remove("+").remove("-").remove("/").remove(".");
    }

    inline QString md5Name(const QByteArray& cont) {
        QString str = QCryptographicHash::hash(cont, QCryptographicHash::Md5)
            .toBase64(QByteArray::OmitTrailingEquals);
        return normalFilename(str);
    }

    QString url2Filename(const QString& url);

};

#endif // _KUTIL_MISC_H_
