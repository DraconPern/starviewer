#include "settings.h"

#include "logging.h"
#include "settingsregistry.h"

#include <QTreeWidget>
#include <QHeaderView> // pel restoreColumnsWidths
#include <QSplitter> // pels saveGeometry(),restoreGeometry() de QSplitter

namespace udg{

Settings::Settings()
{
}

Settings::~Settings()
{
}

QVariant Settings::getValue( const QString &key ) const
{
    QVariant value;
    // Primer mirem si tenim valor als settings
    // Si estigu�s buit, llavors agafem el valor per defecte que tinguem al registre
    value = m_settings.value(key);
    if( value == QVariant() )
    {
        value = SettingsRegistry::instance()->getDefaultValue(key);
    }

    return value;
}

void Settings::setValue( const QString &key, const QVariant &value )
{
    m_settings.setValue(key, value);
}

bool Settings::contains( const QString &key ) const
{
    return m_settings.contains(key);
}

void Settings::remove( const QString &key )
{
    m_settings.remove(key);
}

Settings::KeyValueMapType Settings::getListItem( const QString &key, int index )
{
    KeyValueMapType item;

    int size = m_settings.beginReadArray(key);
    if( index < size && index >= 0 )
    {
        m_settings.setArrayIndex(index);
        // Omplim el conjunt de claus-valor a partir de les claus de l'�ndex de la llista
        item = fillKeyValueMapFromKeyList( m_settings.allKeys() );
    }
    else
    {
        DEBUG_LOG("�ndex fora de rang. L'element retornat ser� buit");
    }

    m_settings.endArray();

    return item;
}

Settings::SettingListType Settings::getList( const QString &key )
{
    SettingListType list;
    int size = m_settings.beginReadArray(key);

    for(int i = 0; i < size; ++i)
    {
        m_settings.setArrayIndex(i);

        KeyValueMapType item;
        // Omplim el conjunt de claus-valor a partir de les claus de l'�ndex de la llista
        item = fillKeyValueMapFromKeyList( m_settings.allKeys() );
        // Afegim el nou conjunts de valors a la llista
        list << item;
    }
    m_settings.endArray();

    return list;
}

void Settings::addListItem( const QString &key, const KeyValueMapType &item )
{
    int arraySize = m_settings.beginReadArray(key);
    m_settings.endArray();
    setListItem( arraySize, key, item );
}

void Settings::setListItem( int index, const QString &key, const KeyValueMapType &item )
{
    // no comprobem si l'�ndex est� dins d'un rang determinat
    // farem servir la pol�tica que tingui QSettings::setArrayIndex()
    m_settings.beginWriteArray(key); 
    m_settings.setArrayIndex(index);
    // omplim
    dumpKeyValueMap(item);
    m_settings.endArray();
}

void Settings::removeListItem( const QString &key, int index )
{
    // ara mateix simplement el que fa �s posar-li la clau adequada
    // TODO mirar si �s necessari fer alguna comprovaci� m�s o si cal "re-ordenar" 
    // la llista, �s a dir, si elimino l'element 3 de 5, potser cal renombrar l'element
    // 4 a "3" i el 5 a "4"    
    remove( key + "/" + QString::number(index+1) );
}

void Settings::setList( const QString &key, const SettingListType &list )
{
    // eliminem tot el que pogu�s haver d'aquella llista anteriorment
    remove(key);
    // escrivim la llista
    m_settings.beginWriteArray(key);
    foreach( KeyValueMapType item, list )
    {
        dumpKeyValueMap( item );
    }
    m_settings.endArray();
}

void Settings::saveColumnsWidths( const QString &key, QTreeWidget *treeWidget )
{
    Q_ASSERT( treeWidget );

    int columnCount = treeWidget->columnCount();
    QString columnKey;
    for( int column = 0; column < columnCount; column++ )
    {   
        columnKey = key + "/columnWidth" + QString::number(column);
        this->setValue( columnKey, treeWidget->columnWidth(column) );
    }
}

void Settings::restoreColumnsWidths( const QString &key, QTreeWidget *treeWidget )
{
    Q_ASSERT( treeWidget );

    int columnCount = treeWidget->columnCount();
    QString columnKey;
    for( int column = 0; column < columnCount; column++ )
    {   
        columnKey = key + "/columnWidth" + QString::number(column);
        if( !this->contains( columnKey ) )
            treeWidget->resizeColumnToContents(column);
        else
            treeWidget->header()->resizeSection( column, this->getValue( columnKey ).toInt() );
    }
}

void Settings::saveGeometry( const QString &key, QWidget *widget )
{
    Q_ASSERT( widget );
    this->setValue( key, widget->saveGeometry() );
}

void Settings::restoreGeometry( const QString &key, QWidget *widget )
{
    Q_ASSERT( widget );
    widget->restoreGeometry( this->getValue(key).toByteArray() );
}

void Settings::saveGeometry( const QString &key, QSplitter *splitter )
{
    Q_ASSERT( splitter );
    this->setValue( key, splitter->saveState() );
}

void Settings::restoreGeometry( const QString &key, QSplitter *splitter )
{
    Q_ASSERT( splitter );
    splitter->restoreState( this->getValue(key).toByteArray() );
}

Settings::KeyValueMapType Settings::fillKeyValueMapFromKeyList( const QStringList &keysList )
{
    KeyValueMapType item;
    
    foreach( QString key, keysList )
    {
        item[ key ] = m_settings.value( key );
    }
    return item;
}

void Settings::dumpKeyValueMap( const KeyValueMapType &item )
{
    QStringList keysList = item.keys();
    foreach( QString key, keysList )
    {
        m_settings.setValue( key, item.value(key) );
    }   
}

}  // end namespace udg
