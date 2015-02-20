/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGHANGINGPROTOCOL_H
#define UDGHANGINGPROTOCOL_H

#include <QMap>
#include <QRegExp>

namespace udg {

class HangingProtocolLayout;
class HangingProtocolMask;
class HangingProtocolImageSet;
class HangingProtocolDisplaySet;

class HangingProtocol {

public:
    HangingProtocol();

    /// Constructor còpia. Fa una còpia de tot, inclòs image i display sets.
    HangingProtocol(const HangingProtocol *hangingProtocol);

    ~HangingProtocol();

    /// Posar el nom al hanging protocol
    void setName(const QString &name);

    /// Definició dels diferents nivells que pot tenir un Hanging Protocol
    enum HangingProtocolLevel { Manufacturer, Site, UserGroup, SingleUser };

    /// Obtenir el layout
    HangingProtocolLayout* getHangingProtocolLayout();

    /// Obtenir la mascara
    HangingProtocolMask* getHangingProtocolMask();

    /// Assigna el nombre de screens
    void setNumberOfScreens(int screens);

    /// Assigna els protocols que tracta el hanging protocol
    void setProtocolsList(const QStringList &protocols);

    /// Assigna les posicions de les finestres
    void setDisplayEnvironmentSpatialPositionList(const QStringList &positions);

    /// Afegeix un image set
    void addImageSet(HangingProtocolImageSet *imageSet);

    /// Afegeix un display set
    void addDisplaySet(HangingProtocolDisplaySet *displaySet);

    /// Obtenir el nom del hanging protocol
    QString getName() const;

    /// Obté el nombre total d'image sets
    int getNumberOfImageSets() const;

    /// Obté el nombre total d'image sets
    int getNumberOfDisplaySets() const;

    /// Obté la llista d'image sets
    QList<HangingProtocolImageSet*> getImageSets() const;

    /// Obté la llista de display sets
    QList<HangingProtocolDisplaySet*> getDisplaySets() const;

    /// Obté l'image set amb identificador "identifier"
    HangingProtocolImageSet* getImageSet(int identifier) const;

    /// Obté el display set amb identificador "identifier"
    HangingProtocolDisplaySet* getDisplaySet(int identifier) const;

    /// Assigna/Obté l'expressió regular del nom de les institucions al que han de pertànyer les imatges
    void setInstitutionsRegularExpression(const QRegExp &institutionRegularExpression);
    QRegExp getInstitutionsRegularExpression() const;

    /// Mètode per mostrar els valors
    void show();

    /// Posar l'identificador al hanging protocol
    void setIdentifier(int id);

    /// Obtenir l'identificador del hanging protocol
    int getIdentifier() const;

    /// Mètode per comparar hanging protocols
    bool isBetterThan(HangingProtocol *hangingToCompare);

    /// Retorna si el mètode és estricte o no ho hes
    bool isStrict() const;

    /// Assigna si el mètode és estricte o no ho hes
    void setStrictness(bool strictness);

    /// Retorna si el hanging protocol ha de tenir totes les series diferents
    bool getAllDiferent() const;

    /// Assigna si el hanging protocol ha de tenir totes les series diferents
    void setAllDiferent(bool allDiferent);

    /// Assigna el tipus d'icona per representar-lo
    void setIconType(const QString &iconType);

    /// Obté el tipus d'icona per representar-lo
    QString getIconType() const;

    /// Posa si el hanging protocol és de previes o no
    void setPrevious(bool isPrevious);

    /// Retorna si el hanging protocol te previes o no
    bool isPrevious() const;

    /// Assigna una prioritat al hanging protocol
    void setPriority(double priority);

    /// Retorna la prioritat del hanging protocol
    double getPriority() const;

    /// Compara si dos hanging protocols son iguals
    bool compareTo(const HangingProtocol &hangingProtocol);

private:
    /// Retorna el número de ImageSets que tenen una sèrie assignada
    int countFilledImageSets() const;

    /// Retorna el número de DisplaySets que tenen una sèrie o imatge assignada
    int countFilledDisplaySets() const;

private:
    /// Identificador
    int m_identifier;

    /// Nom del hanging protocol
    QString m_name;

    /// Descripció del hanging protocol
    QString m_description;

    /// Definició de layouts
    HangingProtocolLayout *m_layout;

    /// Definició de la màscara
    HangingProtocolMask *m_mask;

    QRegExp m_institutionsRegularExpression;

    /// Map from identifier to image set.
    QMap<int, HangingProtocolImageSet*> m_imageSets;

    /// Map from identifier to display set.
    QMap<int, HangingProtocolDisplaySet*> m_displaySets;

    /// Boolea que indica si és estricte o no. Si és estricte vol dir que per ser correcte tots els image sets han d'estar assignats.
    bool m_strictness;

    /// Boolea que indica si les sèries han de ser totes diferents entre elles.
    bool m_allDiferent;

    /// Indica el tipus d'icona per representar el hanging protocol
    QString m_iconType;

    /// Informa si es un hanging protocol amb previes o no
    bool m_hasPrevious;

    /// Prioritat del hanging protocol (per defecte -1)
    double m_priority;
};

}

#endif
