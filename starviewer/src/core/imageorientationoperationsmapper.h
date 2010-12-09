#include <QString>
#include <QMap>

namespace udg {

/**
 *  Aquesta classe s'encarrega de dir-nos quines operacions de rotaci� i flip hem d'aplicar
 *  sobre una imatge 2D donades la orientaci� inicial i la orientaci� desitjada.
 *  
 *  Per fer-ho indicarem la orientaci� inicial i la desitjada a trav�s dels m�todes setInitialOrientation()
 *  i setDesiredOrientation(). Un cop assignades les orientacions preguntarem les operacions que cal 
 *  aplicar sobre la imatge 2D per obtenir la orientaci� que es desitji.
 *
 *  L'ordre en que s'apliquin les operacions indicades *�s �nic* i ha de ser estrictament el seg�ent:
 *      1. Aplicar rotacions
 *      2. Aplicar flips
 *
 *  Les etiquetes d'orientaci� que es proporcionaran seran les corresponents a la part superior i esquerra 
 *  de la imatge. Els valors permesos seran R(right), L(left), A(anterior), P(posterior), H(head) i F(feet).
 *
 *  Aquesta classe nom�s contempla operacions possibles dins d'un mateix pla 2D, per tant no ens pot donar
 *  una f�rmula que passi una imatge adquirida en axial a sagital, per exemple.
 */
class ImageOrientationOperationsMapper {
public:
    ImageOrientationOperationsMapper();
    ~ImageOrientationOperationsMapper();

    /// Indica la orientaci� inicial
    void setInitialOrientation( const QString &topLabel, const QString &leftLabel );

    /// Indica la orientaci� desitjada
    void setDesiredOrientation( const QString &topLabel, const QString &leftLabel );

    /// Ens retorna el n�mero de girs a aplicar en el sentit de les agulles del rellotge
    /// Els valors possibles s�n 0, 1, 2 � 3
    int getNumberOfClockwiseTurnsToApply();
    
    /// Ens indica si cal aplicar un flip horitzontal o no
    bool requiresHorizontalFlip();

private:
    /// Inicialitza els valors de la taula on mapejarem les operacions 
    /// a realitzar segons les possibles orientacions
    void initializeOrientationTable();

    /// Actualitza les operacions a realitzar segons les orientacions introduides
    void updateOperations();

    /// Ens retorna l'etiqueta d'orientaci� en el format intern estipulat
    /// Per exemple, les etiquetes "S" i "I" (Superior i Inferior) es passen als equivalents Head ("H") i Feet ("F")
    QString formatOrientationLabel( const QString &label );

private:
    /// Orientacions inicial i desitjada
    QString m_initialOrientation;
    QString m_desiredOrientation;
    
    /// Nombre de girs a aplicar
    int m_clockwiseTurns;

    /// Indica si cal aplicar flip o no
    bool m_horizontalFlip;

    /// Ens indicar� si cal trobar les rotacions i flips necessaris per la combinaci� actual d'orientacions
    bool m_hasToUpdateOperations;
    
    /// QMap per guardar les operacions de rotaci� i flip corresponents segons les orientacions donades. 
    /// La clau �s una QString en la que guardarem la orientaci� inicial i desitjada en el seg�ent format:
    /// "initialTopLabel\\initialLeftLabel-desiredTopLabel\\desiredLeftLabel"
    /// El valor de cada clau ser� una QString amb el n�mero de rotacions seguida de un valor que indica 
    /// si es fa flip o no, separats per una coma, com per exemple "3,false"
    QMap<QString, QString> m_orientationMappingTable;
};

} // end namespace udg