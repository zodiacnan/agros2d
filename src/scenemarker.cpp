#include "scenemarker.h"

SceneEdgeMarker::SceneEdgeMarker(const QString &name, PhysicFieldBC type)
{
    this->name = name;
    this->type = type;
}

QVariant SceneEdgeMarker::variant()
{
    QVariant v;
    v.setValue(this);
    return v;
}

SceneEdgeMarkerNone::SceneEdgeMarkerNone() : SceneEdgeMarker("none", PHYSICFIELDBC_NONE)
{
}

// *************************************************************************************************************************************

SceneEdgeElectrostaticMarker::SceneEdgeElectrostaticMarker(const QString &name, PhysicFieldBC type, Value value)
        : SceneEdgeMarker(name, type)
{
    this->value = value;
}

QString SceneEdgeElectrostaticMarker::script()
{
    return QString("addBoundary(\"%1\", \"%2\", %3);").
            arg(name).
            arg(physicFieldBCStringKey(type)).
            arg(value.text);
}

QStringList SceneEdgeElectrostaticMarker::data()
{
    QStringList out;
    out << name << physicFieldBCString(type) << value.text;
    return QStringList(out);
}

int SceneEdgeElectrostaticMarker::showDialog(QWidget *parent)
{
    DSceneEdgeElectrostaticMarker *dialog = new DSceneEdgeElectrostaticMarker(this, parent);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneEdgeMagnetostaticMarker::SceneEdgeMagnetostaticMarker(const QString &name, PhysicFieldBC type, Value value) : SceneEdgeMarker(name, type)
{
    this->value = value;
}

QString SceneEdgeMagnetostaticMarker::script()
{
    return QString("addBoundary(\"%1\", \"%2\", %3);").
            arg(name).
            arg(physicFieldBCStringKey(type)).
            arg(value.text);
}

QStringList SceneEdgeMagnetostaticMarker::data()
{
    QStringList out;
    out << name << physicFieldBCString(type) << value.text;
    return QStringList(out);
}

int SceneEdgeMagnetostaticMarker::showDialog(QWidget *parent)
{
    DSceneEdgeMagnetostaticMarker *dialog = new DSceneEdgeMagnetostaticMarker(this, parent);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneEdgeHarmonicMagneticMarker::SceneEdgeHarmonicMagneticMarker(const QString &name, PhysicFieldBC type, Value value) : SceneEdgeMarker(name, type)
{
    this->value = value;
}

QString SceneEdgeHarmonicMagneticMarker::script()
{
    return QString("addBoundary(\"%1\", \"%2\", %3);").
            arg(name).
            arg(physicFieldBCStringKey(type)).
            arg(value.text);
}

QStringList SceneEdgeHarmonicMagneticMarker::data()
{
    QStringList out;
    out << name << physicFieldBCString(type) << value.text;
    return QStringList(out);
}

int SceneEdgeHarmonicMagneticMarker::showDialog(QWidget *parent)
{
    DSceneEdgeHarmonicMagneticMarker *dialog = new DSceneEdgeHarmonicMagneticMarker(this, parent);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneEdgeHeatMarker::SceneEdgeHeatMarker(const QString &name, PhysicFieldBC type, Value temperature) : SceneEdgeMarker(name, type)
{
    this->temperature = temperature;
}

SceneEdgeHeatMarker::SceneEdgeHeatMarker(const QString &name, PhysicFieldBC type, Value heatFlux, Value h, Value externalTemperature) : SceneEdgeMarker(name, type)
{
    this->heatFlux = heatFlux;
    this->h = h;
    this->externalTemperature = externalTemperature;
}

QString SceneEdgeHeatMarker::script()
{
    if (type == PHYSICFIELDBC_HEAT_TEMPERATURE)
    {
        return QString("addBoundary(\"%1\", \"%2\", %3);").
                arg(name).
                arg(physicFieldBCStringKey(type)).
                arg(temperature.text);
    }
    if (type == PHYSICFIELDBC_HEAT_HEAT_FLUX)
    {
        return QString("addBoundary(\"%1\", \"%2\", %3, %4, %5)").
                arg(name).
                arg(physicFieldBCStringKey(type)).
                arg(heatFlux.text).
                arg(h.text).
                arg(externalTemperature.text);
    }
}

QStringList SceneEdgeHeatMarker::data()
{
    QStringList out;
    if (type == PHYSICFIELDBC_HEAT_TEMPERATURE)
    {
        out << name << physicFieldBCString(type) << temperature.text;
    }
    if (type == PHYSICFIELDBC_HEAT_HEAT_FLUX)
    {
        out << name << physicFieldBCString(type) << heatFlux.text << h.text << externalTemperature.text;
    }

    return QStringList(out);
}

int SceneEdgeHeatMarker::showDialog(QWidget *parent)
{
    DSceneEdgeHeatMarker *dialog = new DSceneEdgeHeatMarker(this, parent);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneEdgeCurrentMarker::SceneEdgeCurrentMarker(const QString &name, PhysicFieldBC type, Value value) : SceneEdgeMarker(name, type)
{
    this->value = value;
}

QString SceneEdgeCurrentMarker::script()
{
    return QString("addBoundary(\"%1\", \"%2\", %3);").
            arg(name).
            arg(physicFieldBCStringKey(type)).
            arg(value.text);
}

QStringList SceneEdgeCurrentMarker::data()
{
    QStringList out;
    out << name << physicFieldBCString(type) << value.text;
    return QStringList(out);
}

int SceneEdgeCurrentMarker::showDialog(QWidget *parent)
{
    DSceneEdgeCurrentMarker *dialog = new DSceneEdgeCurrentMarker(this, parent);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneEdgeElasticityMarker::SceneEdgeElasticityMarker(const QString &name, PhysicFieldBC typeX, PhysicFieldBC typeY, double forceX, double forceY)
        : SceneEdgeMarker(name, typeX)
{
    this->typeX = typeX;
    this->typeY = typeY;
    this->forceX = forceX;
    this->forceY = forceY;
}

QString SceneEdgeElasticityMarker::script()
{
    return QString("addEdge(\"%1\", \"%2\", \"%3\", %4, %5);").
            arg(name).
            arg(physicFieldBCStringKey(typeX)).
            arg(physicFieldBCStringKey(typeY)).
            arg(forceX).
            arg(forceY);
}

QStringList SceneEdgeElasticityMarker::data()
{
    QStringList out;
    out << name << physicFieldBCString(typeX) << physicFieldBCString(typeY) << QString::number(forceX) << QString::number(forceY);
    return QStringList(out);
}

int SceneEdgeElasticityMarker::showDialog(QWidget *parent)
{
    DSceneEdgeElasticityMarker *dialog = new DSceneEdgeElasticityMarker(this, parent);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneLabelMarker::SceneLabelMarker(const QString &name)
{
    this->name = name;
}

QVariant SceneLabelMarker::variant()
{
    QVariant v;
    v.setValue(this);
    return v;
}

SceneLabelMarkerNone::SceneLabelMarkerNone() : SceneLabelMarker("none")
{
}

// *************************************************************************************************************************************

SceneLabelElectrostaticMarker::SceneLabelElectrostaticMarker(const QString &name, Value charge_density, Value permittivity)
        : SceneLabelMarker(name)
{
    this->charge_density = charge_density;
    this->permittivity = permittivity;
}

QString SceneLabelElectrostaticMarker::script()
{
    return QString("addMaterial(\"%1\", %2, %3);").
            arg(name).
            arg(charge_density.text).
            arg(permittivity.text);
}

QStringList SceneLabelElectrostaticMarker::data()
{
    QStringList out;
    out << name << charge_density.text << permittivity.text;
    return QStringList(out);
}

int SceneLabelElectrostaticMarker::showDialog(QWidget *parent)
{
    DSceneLabelElectrostaticMarker *dialog = new DSceneLabelElectrostaticMarker(parent, this);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneLabelMagnetostaticMarker::SceneLabelMagnetostaticMarker(const QString &name, Value current_density, Value permeability)
        : SceneLabelMarker(name)
{
    this->permeability = permeability;
    this->current_density = current_density;
}

QString SceneLabelMagnetostaticMarker::script()
{
    return QString("addMaterial(\"%1\", %2, %3);").
            arg(name).
            arg(current_density.text).
            arg(permeability.text);
}

QStringList SceneLabelMagnetostaticMarker::data()
{
    QStringList out;
    out << name << current_density.text << permeability.text;
    return QStringList(out);
}

int SceneLabelMagnetostaticMarker::showDialog(QWidget *parent)
{
    DSceneLabelMagnetostaticMarker *dialog = new DSceneLabelMagnetostaticMarker(parent, this);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneLabelHarmonicMagneticMarker::SceneLabelHarmonicMagneticMarker(const QString &name, Value current_density_real, Value current_density_imag, Value permeability, Value conductivity)
        : SceneLabelMarker(name)
{
    this->permeability = permeability;
    this->conductivity = conductivity;
    this->current_density_real = current_density_real;
    this->current_density_imag = current_density_imag;
}

QString SceneLabelHarmonicMagneticMarker::script()
{
    return QString("addMaterial(\"%1\", %2, %3, %4, %5);").
            arg(name).
            arg(current_density_real.text).
            arg(current_density_imag.text).
            arg(permeability.text).
            arg(conductivity.text);
}

QStringList SceneLabelHarmonicMagneticMarker::data()
{
    QStringList out;
    out << name << current_density_real.text << current_density_imag.text << permeability.text << conductivity.text;
    return QStringList(out);
}

int SceneLabelHarmonicMagneticMarker::showDialog(QWidget *parent)
{
    DSceneLabelHarmonicMagneticMarker *dialog = new DSceneLabelHarmonicMagneticMarker(parent, this);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneLabelCurrentMarker::SceneLabelCurrentMarker(const QString &name, Value conductivity) : SceneLabelMarker(name)
{
    this->conductivity = conductivity;
}

QString SceneLabelCurrentMarker::script()
{
    return QString("addMaterial(\"%1\", %3);").
            arg(name).
            arg(conductivity.text);
}

QStringList SceneLabelCurrentMarker::data()
{
    QStringList out;
    out << name << conductivity.text;
    return QStringList(out);
}

int SceneLabelCurrentMarker::showDialog(QWidget *parent)
{
    DSceneLabelCurrentMarker *dialog = new DSceneLabelCurrentMarker(parent, this);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneLabelHeatMarker::SceneLabelHeatMarker(const QString &name, Value volume_heat, Value thermal_conductivity)
        : SceneLabelMarker(name)
{
    this->thermal_conductivity = thermal_conductivity;
    this->volume_heat = volume_heat;
}

QString SceneLabelHeatMarker::script()
{
    return QString("addMaterial(\"%1\", %2, %3);").
            arg(name).
            arg(volume_heat.text).
            arg(thermal_conductivity.text);
}

QStringList SceneLabelHeatMarker::data()
{
    QStringList out;
    out << name << volume_heat.text << thermal_conductivity.text;
    return QStringList(out);
}

int SceneLabelHeatMarker::showDialog(QWidget *parent)
{
    DSceneLabelHeatMarker *dialog = new DSceneLabelHeatMarker(parent, this);
    return dialog->exec();
}

// *************************************************************************************************************************************

SceneLabelElasticityMarker::SceneLabelElasticityMarker(const QString &name, double young_modulus, double poisson_ratio)
        : SceneLabelMarker(name)
{
    this->young_modulus = young_modulus;
    this->poisson_ratio = poisson_ratio;
}

QString SceneLabelElasticityMarker::script()
{
    return QString("addMaterial(\"%1\", %2, %3);").
            arg(name).
            arg(young_modulus).
            arg(poisson_ratio);
}

QStringList SceneLabelElasticityMarker::data()
{
    QStringList out;
    out << name << QString::number(young_modulus) << QString::number(poisson_ratio);
    return QStringList(out);
}

int SceneLabelElasticityMarker::showDialog(QWidget *parent)
{
    DSceneLabelElasticityMarker *dialog = new DSceneLabelElasticityMarker(parent, this);
    return dialog->exec();
}

// *************************************************************************************************************************************

DSceneEdgeMarker::DSceneEdgeMarker(QWidget *parent) : QDialog(parent)
{
    layout = new QVBoxLayout();
    txtName = new QLineEdit("");
}

DSceneEdgeMarker::~DSceneEdgeMarker()
{
    delete layout;
    delete txtName;
}

void DSceneEdgeMarker::createDialog()
{
    QHBoxLayout *layoutName = new QHBoxLayout();
    layoutName->addWidget(new QLabel(tr("Name:")));
    layoutName->addWidget(txtName);

    // dialog buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(doAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(doReject()));

    layout->addLayout(layoutName);
    layout->addLayout(createContent());
    layout->addStretch();
    layout->addWidget(buttonBox);

    txtName->setFocus();

    setLayout(layout);
}

void DSceneEdgeMarker::load()
{
    txtName->setText(m_edgeMarker->name);
}

bool DSceneEdgeMarker::save()
{
    // find name duplicities
    foreach (SceneEdgeMarker *edgeMarker, Util::scene()->edgeMarkers)
    {
        if (edgeMarker->name == txtName->text())
        {
            if (m_edgeMarker == edgeMarker)
                continue;

            QMessageBox::warning(this, tr("Boundary marker"), tr("Boundary marker name already exists."));
            return false;
        }
    }
    m_edgeMarker->name = txtName->text();
    return true;
}

void DSceneEdgeMarker::setSize()
{
    setWindowIcon(icon("scene-edgemarker"));
    setWindowTitle(tr("Boundary condition"));

    resize(sizeHint());
    setMinimumSize(sizeHint());
    setMaximumSize(sizeHint());
}

void DSceneEdgeMarker::doAccept()
{
    if (save())
        accept();    
}

void DSceneEdgeMarker::doReject()
{
    reject();
}

// *************************************************************************************************************************************

DSceneEdgeElectrostaticMarker::DSceneEdgeElectrostaticMarker(SceneEdgeElectrostaticMarker *edgeElectrostaticMarker, QWidget *parent) : DSceneEdgeMarker(parent)
{
    m_edgeMarker = edgeElectrostaticMarker;

    createDialog();

    // tab order
    setTabOrder(txtName, cmbType);
    setTabOrder(cmbType, txtValue);

    load();
    setSize();
}

DSceneEdgeElectrostaticMarker::~DSceneEdgeElectrostaticMarker()
{
    delete cmbType;
    delete txtValue;
}

QLayout* DSceneEdgeElectrostaticMarker::createContent()
{
    cmbType = new QComboBox();
    cmbType->addItem("none", PHYSICFIELDBC_NONE);
    cmbType->addItem(physicFieldBCString(PHYSICFIELDBC_ELECTROSTATIC_POTENTIAL), PHYSICFIELDBC_ELECTROSTATIC_POTENTIAL);
    cmbType->addItem(physicFieldBCString(PHYSICFIELDBC_ELECTROSTATIC_SURFACE_CHARGE), PHYSICFIELDBC_ELECTROSTATIC_SURFACE_CHARGE);

    txtValue = new SLineEditValue(this);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("BC type:"), cmbType);
    layoutMarker->addRow(tr("Value:"), txtValue);

    return layoutMarker;
}

void DSceneEdgeElectrostaticMarker::load()
{
    DSceneEdgeMarker::load();

    SceneEdgeElectrostaticMarker *edgeElectrostaticMarker = dynamic_cast<SceneEdgeElectrostaticMarker *>(m_edgeMarker);

    cmbType->setCurrentIndex(cmbType->findData(edgeElectrostaticMarker->type));
    txtValue->setText(edgeElectrostaticMarker->value.text);
}

bool DSceneEdgeElectrostaticMarker::save() {
    if (!DSceneEdgeMarker::save()) return false;

    SceneEdgeElectrostaticMarker *edgeElectrostaticMarker = dynamic_cast<SceneEdgeElectrostaticMarker *>(m_edgeMarker);

    edgeElectrostaticMarker->type = (PhysicFieldBC) cmbType->itemData(cmbType->currentIndex()).toInt();

    if (txtValue->evaluate())
        edgeElectrostaticMarker->value  = txtValue->value();
    else
        return false;

    return true;
}

// *************************************************************************************************************************************

DSceneEdgeMagnetostaticMarker::DSceneEdgeMagnetostaticMarker(SceneEdgeMagnetostaticMarker *edgeMagnetostaticMarker, QWidget *parent) : DSceneEdgeMarker(parent)
{
    m_edgeMarker = edgeMagnetostaticMarker;

    createDialog();

    // tab order
    setTabOrder(txtName, cmbType);
    setTabOrder(cmbType, txtValue);

    load();
    setSize();
}

DSceneEdgeMagnetostaticMarker::~DSceneEdgeMagnetostaticMarker()
{
    delete cmbType;
    delete txtValue;
}

QLayout* DSceneEdgeMagnetostaticMarker::createContent()
{
    cmbType = new QComboBox();
    cmbType->addItem("none", PHYSICFIELDBC_NONE);
    cmbType->addItem(physicFieldBCString(PHYSICFIELDBC_MAGNETOSTATIC_VECTOR_POTENTIAL), PHYSICFIELDBC_MAGNETOSTATIC_VECTOR_POTENTIAL);
    cmbType->addItem(physicFieldBCString(PHYSICFIELDBC_MAGNETOSTATIC_SURFACE_CURRENT), PHYSICFIELDBC_MAGNETOSTATIC_SURFACE_CURRENT);

    txtValue = new SLineEditValue(this);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("BC type:"), cmbType);
    layoutMarker->addRow(tr("Value:"), txtValue);

    return layoutMarker;
}

void DSceneEdgeMagnetostaticMarker::load()
{
    DSceneEdgeMarker::load();

    SceneEdgeMagnetostaticMarker *edgeMagnetostaticMarker = dynamic_cast<SceneEdgeMagnetostaticMarker *>(m_edgeMarker);

    cmbType->setCurrentIndex(cmbType->findData(edgeMagnetostaticMarker->type));
    txtValue->setText(edgeMagnetostaticMarker->value.text);
}

bool DSceneEdgeMagnetostaticMarker::save() {
    if (!DSceneEdgeMarker::save()) return false;;

    SceneEdgeMagnetostaticMarker *edgeMagnetostaticMarker = dynamic_cast<SceneEdgeMagnetostaticMarker *>(m_edgeMarker);

    edgeMagnetostaticMarker->type = (PhysicFieldBC) cmbType->itemData(cmbType->currentIndex()).toInt();

    if (txtValue->evaluate())
        edgeMagnetostaticMarker->value  = txtValue->value();
    else
        return false;

    return true;
}


// *************************************************************************************************************************************

DSceneEdgeHarmonicMagneticMarker::DSceneEdgeHarmonicMagneticMarker(SceneEdgeHarmonicMagneticMarker *edgeHarmonicMagneticMarker, QWidget *parent) : DSceneEdgeMarker(parent)
{
    m_edgeMarker = edgeHarmonicMagneticMarker;

    createDialog();

    // tab order
    setTabOrder(txtName, cmbType);
    setTabOrder(cmbType, txtValue);

    load();
    setSize();
}

DSceneEdgeHarmonicMagneticMarker::~DSceneEdgeHarmonicMagneticMarker()
{
    delete cmbType;
    delete txtValue;
}

QLayout* DSceneEdgeHarmonicMagneticMarker::createContent()
{
    cmbType = new QComboBox();
    cmbType->addItem("none", PHYSICFIELDBC_NONE);
    cmbType->addItem(physicFieldBCString(PHYSICFIELDBC_HARMONIC_MAGNETIC_VECTOR_POTENTIAL), PHYSICFIELDBC_HARMONIC_MAGNETIC_VECTOR_POTENTIAL);
    cmbType->addItem(physicFieldBCString(PHYSICFIELDBC_HARMONIC_MAGNETIC_SURFACE_CURRENT), PHYSICFIELDBC_HARMONIC_MAGNETIC_SURFACE_CURRENT);

    txtValue = new SLineEditValue(this);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("BC type:"), cmbType);
    layoutMarker->addRow(tr("Value:"), txtValue);

    return layoutMarker;
}

void DSceneEdgeHarmonicMagneticMarker::load()
{
    DSceneEdgeMarker::load();

    SceneEdgeHarmonicMagneticMarker *edgeHarmonicMagneticMarker = dynamic_cast<SceneEdgeHarmonicMagneticMarker *>(m_edgeMarker);

    cmbType->setCurrentIndex(cmbType->findData(edgeHarmonicMagneticMarker->type));
    txtValue->setText(edgeHarmonicMagneticMarker->value.text);
}

bool DSceneEdgeHarmonicMagneticMarker::save() {
    if (!DSceneEdgeMarker::save()) return false;;

    SceneEdgeHarmonicMagneticMarker *edgeHarmonicMagneticMarker = dynamic_cast<SceneEdgeHarmonicMagneticMarker *>(m_edgeMarker);

    edgeHarmonicMagneticMarker->type = (PhysicFieldBC) cmbType->itemData(cmbType->currentIndex()).toInt();

    if (txtValue->evaluate())
        edgeHarmonicMagneticMarker->value  = txtValue->value();
    else
        return false;

    return true;
}

// *************************************************************************************************************************************

DSceneEdgeHeatMarker::DSceneEdgeHeatMarker(SceneEdgeHeatMarker *edgeEdgeHeatMarker, QWidget *parent) : DSceneEdgeMarker(parent)
{
    m_edgeMarker = edgeEdgeHeatMarker;

    createDialog();

    // tab order
    setTabOrder(txtName, cmbType);
    setTabOrder(cmbType, txtTemperature);
    setTabOrder(txtTemperature, txtHeatFlux);
    setTabOrder(txtHeatFlux, txtHeatTransferCoefficient);
    setTabOrder(txtHeatTransferCoefficient, txtExternalTemperature);

    load();
    setSize();
}

DSceneEdgeHeatMarker::~DSceneEdgeHeatMarker()
{
    delete cmbType;
    delete txtTemperature;
    delete txtHeatFlux;
    delete txtHeatTransferCoefficient;
    delete txtExternalTemperature;
}

QLayout* DSceneEdgeHeatMarker::createContent()
{
    cmbType = new QComboBox();
    cmbType->addItem("none", PHYSICFIELDBC_NONE);
    cmbType->addItem(physicFieldBCString(PHYSICFIELDBC_HEAT_TEMPERATURE), PHYSICFIELDBC_HEAT_TEMPERATURE);
    cmbType->addItem(physicFieldBCString(PHYSICFIELDBC_HEAT_HEAT_FLUX), PHYSICFIELDBC_HEAT_HEAT_FLUX);
    connect(cmbType, SIGNAL(currentIndexChanged(int)), this, SLOT(doTypeChanged(int)));

    txtHeatFlux = new SLineEditValue(this);
    txtTemperature = new SLineEditValue(this);
    txtHeatTransferCoefficient = new SLineEditValue(this);
    txtExternalTemperature = new SLineEditValue(this);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("BC Type:"), cmbType);
    layoutMarker->addRow(tr("Temperature:"), txtTemperature);
    layoutMarker->addRow(tr("Heat flux:"), txtHeatFlux);
    layoutMarker->addRow(tr("Heat transfer coef.:"), txtHeatTransferCoefficient);
    layoutMarker->addRow(tr("External temperature:"), txtExternalTemperature);

    return layoutMarker;
}

void DSceneEdgeHeatMarker::load()
{
    DSceneEdgeMarker::load();

    SceneEdgeHeatMarker *edgeHeatMarker = dynamic_cast<SceneEdgeHeatMarker *>(m_edgeMarker);

    cmbType->setCurrentIndex(cmbType->findData(edgeHeatMarker->type));
    switch (edgeHeatMarker->type)
    {
    case PHYSICFIELDBC_HEAT_TEMPERATURE:
        {
            txtTemperature->setText(edgeHeatMarker->temperature.text);
        }
        break;
    case PHYSICFIELDBC_HEAT_HEAT_FLUX:
        {
            txtHeatFlux->setText(edgeHeatMarker->heatFlux.text);
            txtHeatTransferCoefficient->setText(edgeHeatMarker->h.text);
            txtExternalTemperature->setText(edgeHeatMarker->externalTemperature.text);
        }
        break;
    }
}

bool DSceneEdgeHeatMarker::save() {
    if (!DSceneEdgeMarker::save()) return false;;

    SceneEdgeHeatMarker *edgeHeatMarker = dynamic_cast<SceneEdgeHeatMarker *>(m_edgeMarker);

    edgeHeatMarker->type = (PhysicFieldBC) cmbType->itemData(cmbType->currentIndex()).toInt();
    switch (edgeHeatMarker->type)
    {
    case PHYSICFIELDBC_HEAT_TEMPERATURE:
        {
            if (txtTemperature->evaluate())
                edgeHeatMarker->temperature  = txtTemperature->value();
            else
                return false;
        }
        break;
    case PHYSICFIELDBC_HEAT_HEAT_FLUX:
        {
            if (txtHeatFlux->evaluate())
                edgeHeatMarker->heatFlux  = txtHeatFlux->value();
            else
                return false;
            if (txtHeatTransferCoefficient->evaluate())
                edgeHeatMarker->h  = txtHeatTransferCoefficient->value();
            else
                return false;
            if (txtExternalTemperature->evaluate())
                edgeHeatMarker->externalTemperature  = txtExternalTemperature->value();
            else
                return false;
        }
        break;
    }

    return true;
}

void DSceneEdgeHeatMarker::doTypeChanged(int index)
{
    txtTemperature->setEnabled(false);
    txtHeatFlux->setEnabled(false);
    txtHeatTransferCoefficient->setEnabled(false);
    txtExternalTemperature->setEnabled(false);

    switch ((PhysicFieldBC) cmbType->itemData(index).toInt())
    {
    case PHYSICFIELDBC_HEAT_TEMPERATURE:
        {
            txtTemperature->setEnabled(true);
        }
        break;
    case PHYSICFIELDBC_HEAT_HEAT_FLUX:
        {
            txtHeatFlux->setEnabled(true);
            txtHeatTransferCoefficient->setEnabled(true);
            txtExternalTemperature->setEnabled(true);
        }
        break;
    }
}

// *************************************************************************************************************************************

DSceneEdgeCurrentMarker::DSceneEdgeCurrentMarker(SceneEdgeCurrentMarker *edgeCurrentMarker, QWidget *parent) : DSceneEdgeMarker(parent)
{
    m_edgeMarker = edgeCurrentMarker;

    createDialog(); 

    // tab order
    setTabOrder(txtName, cmbType);
    setTabOrder(cmbType, txtValue);

    load();
    setSize();
}

DSceneEdgeCurrentMarker::~DSceneEdgeCurrentMarker()
{
    delete cmbType;
    delete txtValue;
}

QLayout* DSceneEdgeCurrentMarker::createContent()
{
    cmbType = new QComboBox();
    cmbType->addItem("none", PHYSICFIELDBC_NONE);
    cmbType->addItem(physicFieldBCString(PHYSICFIELDBC_CURRENT_POTENTIAL), PHYSICFIELDBC_CURRENT_POTENTIAL);
    cmbType->addItem(physicFieldBCString(PHYSICFIELDBC_CURRENT_INWARD_CURRENT_FLOW), PHYSICFIELDBC_CURRENT_INWARD_CURRENT_FLOW);

    txtValue = new SLineEditValue(this);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("BC type:"), cmbType);
    layoutMarker->addRow(tr("Value:"), txtValue);

    return layoutMarker;
}

void DSceneEdgeCurrentMarker::load()
{
    DSceneEdgeMarker::load();

    SceneEdgeCurrentMarker *edgeCurrentMarker = dynamic_cast<SceneEdgeCurrentMarker *>(m_edgeMarker);

    cmbType->setCurrentIndex(cmbType->findData(edgeCurrentMarker->type));
    txtValue->setText(edgeCurrentMarker->value.text);
}

bool DSceneEdgeCurrentMarker::save() {
    if (!DSceneEdgeMarker::save()) return false;;

    SceneEdgeCurrentMarker *edgeCurrentMarker = dynamic_cast<SceneEdgeCurrentMarker *>(m_edgeMarker);

    edgeCurrentMarker->type = (PhysicFieldBC) cmbType->itemData(cmbType->currentIndex()).toInt();

    Value value = Value(txtValue->text());
    if (value.evaluate(Util::scene()->problemInfo().scriptStartup))
        edgeCurrentMarker->value = value;
    else
        return false;

    return true;
}

// *************************************************************************************************************************************

DSceneEdgeElasticityMarker::DSceneEdgeElasticityMarker(SceneEdgeElasticityMarker *edgeEdgeElasticityMarker, QWidget *parent) : DSceneEdgeMarker(parent)
{
    m_edgeMarker = edgeEdgeElasticityMarker;

    createDialog();

    // tab order
    setTabOrder(txtName, cmbTypeX);
    setTabOrder(cmbTypeX, txtForceX);

    load();
    setSize();
}

DSceneEdgeElasticityMarker::~DSceneEdgeElasticityMarker()
{
    delete cmbTypeX;
    delete cmbTypeY;
    delete txtForceX;
    delete txtForceY;
}

QLayout* DSceneEdgeElasticityMarker::createContent()
{
    cmbTypeX = new QComboBox();
    cmbTypeX->addItem("none", PHYSICFIELDBC_NONE);
    cmbTypeX->addItem(physicFieldBCString(PHYSICFIELDBC_ELASTICITY_FREE), PHYSICFIELDBC_ELASTICITY_FREE);
    cmbTypeX->addItem(physicFieldBCString(PHYSICFIELDBC_ELASTICITY_FIXED), PHYSICFIELDBC_ELASTICITY_FIXED);

    cmbTypeY = new QComboBox();
    cmbTypeY->addItem("none", PHYSICFIELDBC_NONE);
    cmbTypeY->addItem(physicFieldBCString(PHYSICFIELDBC_ELASTICITY_FREE), PHYSICFIELDBC_ELASTICITY_FREE);
    cmbTypeY->addItem(physicFieldBCString(PHYSICFIELDBC_ELASTICITY_FIXED), PHYSICFIELDBC_ELASTICITY_FIXED);

    txtForceX = new SLineEdit("0", false);
    txtForceY = new SLineEdit("0", false);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("BC Type X:"), cmbTypeX);
    layoutMarker->addRow(tr("BC Type Y:"), cmbTypeY);
    layoutMarker->addRow(tr("Force X:"), txtForceX);
    layoutMarker->addRow(tr("Force Y:"), txtForceY);

    return layoutMarker;
}

void DSceneEdgeElasticityMarker::load()
{
    DSceneEdgeMarker::load();

    SceneEdgeElasticityMarker *edgeElasticityMarker = dynamic_cast<SceneEdgeElasticityMarker *>(m_edgeMarker);

    cmbTypeX->setCurrentIndex(cmbTypeX->findData(edgeElasticityMarker->typeX));
    cmbTypeY->setCurrentIndex(cmbTypeY->findData(edgeElasticityMarker->typeY));

    txtForceX->setText(QString::number(edgeElasticityMarker->forceX));
    txtForceY->setText(QString::number(edgeElasticityMarker->forceY));
}

bool DSceneEdgeElasticityMarker::save() {
    if (!DSceneEdgeMarker::save()) return false;;

    SceneEdgeElasticityMarker *edgeElasticityMarker = dynamic_cast<SceneEdgeElasticityMarker *>(m_edgeMarker);

    edgeElasticityMarker->typeX = (PhysicFieldBC) cmbTypeX->itemData(cmbTypeX->currentIndex()).toInt();
    edgeElasticityMarker->typeY = (PhysicFieldBC) cmbTypeY->itemData(cmbTypeY->currentIndex()).toInt();
    edgeElasticityMarker->forceX = txtForceX->text().toDouble();
    edgeElasticityMarker->forceY = txtForceY->text().toDouble();

    return true;
}

// *************************************************************************************************************************************

DSceneLabelMarker::DSceneLabelMarker(QWidget *parent) : QDialog(parent)
{
    layout = new QVBoxLayout();
    txtName = new QLineEdit("");
}

DSceneLabelMarker::~DSceneLabelMarker()
{
    delete layout;
    delete txtName;
}

void DSceneLabelMarker::createDialog()
{
    QHBoxLayout *layoutName = new QHBoxLayout();
    layoutName->addWidget(new QLabel(tr("Name:")));
    layoutName->addWidget(txtName);

    // dialog buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(doAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(doReject()));

    layout->addLayout(layoutName);
    layout->addLayout(createContent());
    layout->addStretch();
    layout->addWidget(buttonBox);

    txtName->setFocus();

    setLayout(layout);
}

void DSceneLabelMarker::setSize()
{
    setWindowIcon(icon("scene-labelmarker"));
    setWindowTitle(tr("Material"));

    resize(sizeHint());
    setMinimumSize(sizeHint());
    setMaximumSize(sizeHint());
}

void DSceneLabelMarker::load()
{
    txtName->setText(m_labelMarker->name);
}

bool DSceneLabelMarker::save()
{
    // find name duplicities
    foreach (SceneLabelMarker *labelMarker, Util::scene()->labelMarkers)
    {
        if (labelMarker->name == txtName->text())
        {
            if (m_labelMarker == labelMarker)
                continue;

            QMessageBox::warning(this, tr("Material marker"), tr("Material marker name already exists."));
            return false;
        }
    }
    m_labelMarker->name = txtName->text();
    return true;
}

void DSceneLabelMarker::doAccept()
{
    if (save())
        accept();
}

void DSceneLabelMarker::doReject()
{
    reject();
}

// *************************************************************************************************************************************

DSceneLabelElectrostaticMarker::DSceneLabelElectrostaticMarker(QWidget *parent, SceneLabelElectrostaticMarker *labelElectrostaticMarker) : DSceneLabelMarker(parent)
{
    m_labelMarker = labelElectrostaticMarker;

    createDialog();

    // tab order
    setTabOrder(txtName, txtPermittivity);
    setTabOrder(txtPermittivity, txtChargeDensity);

    load();
    setSize();
}

DSceneLabelElectrostaticMarker::~DSceneLabelElectrostaticMarker()
{
    delete txtPermittivity;
    delete txtChargeDensity;
}

QLayout* DSceneLabelElectrostaticMarker::createContent()
{
    txtPermittivity = new SLineEditValue(this);
    txtChargeDensity = new SLineEditValue(this);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("Permittivity (-):"), txtPermittivity);
    layoutMarker->addRow(tr("Charge density (C/m3):"), txtChargeDensity);

    return layoutMarker;
}

void DSceneLabelElectrostaticMarker::load()
{
    DSceneLabelMarker::load();

    SceneLabelElectrostaticMarker *labelElectrostaticMarker = dynamic_cast<SceneLabelElectrostaticMarker *>(m_labelMarker);

    txtPermittivity->setText(labelElectrostaticMarker->permittivity.text);
    txtChargeDensity->setText(labelElectrostaticMarker->charge_density.text);
}

bool DSceneLabelElectrostaticMarker::save() {
    if (!DSceneLabelMarker::save()) return false;;

    SceneLabelElectrostaticMarker *labelElectrostaticMarker = dynamic_cast<SceneLabelElectrostaticMarker *>(m_labelMarker);

    if (txtPermittivity->evaluate())
        labelElectrostaticMarker->permittivity = txtPermittivity->value();
    else
        return false;

    if (txtChargeDensity->evaluate())
        labelElectrostaticMarker->charge_density = txtChargeDensity->value();
    else
        return false;

    return true;
}

// *************************************************************************************************************************************

DSceneLabelMagnetostaticMarker::DSceneLabelMagnetostaticMarker(QWidget *parent, SceneLabelMagnetostaticMarker *labelMagnetostaticMarker) : DSceneLabelMarker(parent)
{
    m_labelMarker = labelMagnetostaticMarker;

    createDialog();

    // tab order
    setTabOrder(txtName, txtPermeability);
    setTabOrder(txtPermeability, txtCurrentDensity);

    load();
    setSize();
}

DSceneLabelMagnetostaticMarker::~DSceneLabelMagnetostaticMarker()
{
    delete txtPermeability;
    delete txtCurrentDensity;
}

QLayout* DSceneLabelMagnetostaticMarker::createContent()
{
    txtPermeability = new SLineEditValue(this);
    txtCurrentDensity = new SLineEditValue(this);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("Permeability (-):"), txtPermeability);
    layoutMarker->addRow(tr("Current density (A/m2):"), txtCurrentDensity);

    return layoutMarker;
}

void DSceneLabelMagnetostaticMarker::load()
{
    DSceneLabelMarker::load();

    SceneLabelMagnetostaticMarker *labelMagnetostaticMarker = dynamic_cast<SceneLabelMagnetostaticMarker *>(m_labelMarker);

    txtPermeability->setText(labelMagnetostaticMarker->permeability.text);
    txtCurrentDensity->setText(labelMagnetostaticMarker->current_density.text);
}

bool DSceneLabelMagnetostaticMarker::save() {
    if (!DSceneLabelMarker::save()) return false;;

    SceneLabelMagnetostaticMarker *labelMagnetostaticMarker = dynamic_cast<SceneLabelMagnetostaticMarker *>(m_labelMarker);

    if (txtPermeability->evaluate())
        labelMagnetostaticMarker->permeability  = txtPermeability->value();
    else
        return false;

    if (txtCurrentDensity->evaluate())
        labelMagnetostaticMarker->current_density  = txtCurrentDensity->value();
    else
        return false;

    return true;
}

// *************************************************************************************************************************************

DSceneLabelHarmonicMagneticMarker::DSceneLabelHarmonicMagneticMarker(QWidget *parent, SceneLabelHarmonicMagneticMarker *labelHarmonicMagneticMarker) : DSceneLabelMarker(parent)
{
    m_labelMarker = labelHarmonicMagneticMarker;

    createDialog();

    // tab order
    setTabOrder(txtName, txtPermeability);
    setTabOrder(txtPermeability, txtConductivity);
    setTabOrder(txtConductivity, txtCurrentDensityReal);
    setTabOrder(txtCurrentDensityReal, txtCurrentDensityImag);

    load();
    setSize();
}

DSceneLabelHarmonicMagneticMarker::~DSceneLabelHarmonicMagneticMarker()
{
    delete txtPermeability;
    delete txtConductivity;
    delete txtCurrentDensityReal;
    delete txtCurrentDensityImag;
}

QLayout* DSceneLabelHarmonicMagneticMarker::createContent()
{
    txtPermeability = new SLineEditValue(this);
    txtConductivity = new SLineEditValue(this);
    txtCurrentDensityReal = new SLineEditValue(this);
    txtCurrentDensityImag = new SLineEditValue(this);

    QHBoxLayout *layoutCurrentDensity = new QHBoxLayout();
    layoutCurrentDensity->addWidget(txtCurrentDensityReal);
    layoutCurrentDensity->addWidget(new QLabel(" + j "));
    layoutCurrentDensity->addWidget(txtCurrentDensityImag);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("Permeability (-):"), txtPermeability);
    layoutMarker->addRow(tr("Conductivity (-):"), txtConductivity);
    layoutMarker->addRow(tr("Current density (A/m2):"), layoutCurrentDensity);

    return layoutMarker;
}

void DSceneLabelHarmonicMagneticMarker::load()
{
    DSceneLabelMarker::load();

    SceneLabelHarmonicMagneticMarker *labelHarmonicMagneticMarker = dynamic_cast<SceneLabelHarmonicMagneticMarker *>(m_labelMarker);

    txtPermeability->setText(labelHarmonicMagneticMarker->permeability.text);
    txtConductivity->setText(labelHarmonicMagneticMarker->conductivity.text);
    txtCurrentDensityReal->setText(labelHarmonicMagneticMarker->current_density_real.text);
    txtCurrentDensityImag->setText(labelHarmonicMagneticMarker->current_density_imag.text);
}

bool DSceneLabelHarmonicMagneticMarker::save() {
    if (!DSceneLabelMarker::save()) return false;;

    SceneLabelHarmonicMagneticMarker *labelHarmonicMagneticMarker = dynamic_cast<SceneLabelHarmonicMagneticMarker *>(m_labelMarker);

    if (txtPermeability->evaluate())
        labelHarmonicMagneticMarker->permeability  = txtPermeability->value();
    else
        return false;

    if (txtConductivity->evaluate())
        labelHarmonicMagneticMarker->conductivity  = txtConductivity->value();
    else
        return false;

    if (txtCurrentDensityReal->evaluate())
        labelHarmonicMagneticMarker->current_density_real  = txtCurrentDensityReal->value();
    else
        return false;

    if (txtCurrentDensityImag->evaluate())
        labelHarmonicMagneticMarker->current_density_imag  = txtCurrentDensityImag->value();
    else
        return false;

    return true;
}

// *************************************************************************************************************************************

DSceneLabelHeatMarker::DSceneLabelHeatMarker(QWidget *parent, SceneLabelHeatMarker *labelHeatMarker) : DSceneLabelMarker(parent)
{
    m_labelMarker = labelHeatMarker;

    createDialog();

    // tab order
    setTabOrder(txtName, txtThermalConductivity);
    setTabOrder(txtThermalConductivity, txtVolumeHeat);

    load();
    setSize();
}

DSceneLabelHeatMarker::~DSceneLabelHeatMarker()
{
    delete txtThermalConductivity;
    delete txtVolumeHeat;
}

QLayout* DSceneLabelHeatMarker::createContent()
{
    txtThermalConductivity = new SLineEditValue(this);
    txtVolumeHeat = new SLineEditValue(this);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("Thermal conductivity (W/m.K):"), txtThermalConductivity);
    layoutMarker->addRow(tr("Volume heat (J/m3):"), txtVolumeHeat);

    return layoutMarker;
}

void DSceneLabelHeatMarker::load()
{
    DSceneLabelMarker::load();

    SceneLabelHeatMarker *labelHeatMarker = dynamic_cast<SceneLabelHeatMarker *>(m_labelMarker);

    txtThermalConductivity->setText(labelHeatMarker->thermal_conductivity.text);
    txtVolumeHeat->setText(labelHeatMarker->volume_heat.text);
}

bool DSceneLabelHeatMarker::save()
{
    if (!DSceneLabelMarker::save()) return false;;

    SceneLabelHeatMarker *labelHeatMarker = dynamic_cast<SceneLabelHeatMarker *>(m_labelMarker);

    if (txtThermalConductivity->evaluate())
        labelHeatMarker->thermal_conductivity  = txtThermalConductivity->value();
    else
        return false;

    if (txtVolumeHeat->evaluate())
        labelHeatMarker->volume_heat  = txtVolumeHeat->value();
    else
        return false;

    return true;
}

// *************************************************************************************************************************************

DSceneLabelCurrentMarker::DSceneLabelCurrentMarker(QWidget *parent, SceneLabelCurrentMarker *labelCurrentMarker) : DSceneLabelMarker(parent)
{
    m_labelMarker = labelCurrentMarker;

    createDialog();

    // tab order
    setTabOrder(txtName, txtConductivity);

    load();
    setSize();
}

DSceneLabelCurrentMarker::~DSceneLabelCurrentMarker()
{
    delete txtConductivity;
}

QLayout* DSceneLabelCurrentMarker::createContent()
{
    txtConductivity = new SLineEditValue(this);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("Conductivity (S/m):"), txtConductivity);

    return layoutMarker;
}

void DSceneLabelCurrentMarker::load()
{
    DSceneLabelMarker::load();

    SceneLabelCurrentMarker *labelCurrentMarker = dynamic_cast<SceneLabelCurrentMarker *>(m_labelMarker);

    txtConductivity->setText(labelCurrentMarker->conductivity.text);
}

bool DSceneLabelCurrentMarker::save()
{
    if (!DSceneLabelMarker::save()) return false;;

    SceneLabelCurrentMarker *labelCurrentMarker = dynamic_cast<SceneLabelCurrentMarker *>(m_labelMarker);

    if (txtConductivity->evaluate())
        labelCurrentMarker->conductivity  = txtConductivity->value();
    else
        return false;

    return true;
}

// *************************************************************************************************************************************

DSceneLabelElasticityMarker::DSceneLabelElasticityMarker(QWidget *parent, SceneLabelElasticityMarker *labelElasticityMarker) : DSceneLabelMarker(parent)
{
    m_labelMarker = labelElasticityMarker;

    createDialog();

    // tab order
    setTabOrder(txtName, txtYoungModulus);
    setTabOrder(txtYoungModulus, txtPoissonNumber);

    load();
    setSize();
}

DSceneLabelElasticityMarker::~DSceneLabelElasticityMarker()
{
    delete txtYoungModulus;
    delete txtPoissonNumber;
}

QLayout* DSceneLabelElasticityMarker::createContent()
{
    txtYoungModulus = new SLineEdit("0", false);
    txtPoissonNumber = new SLineEdit("0", false);

    QFormLayout *layoutMarker = new QFormLayout();
    layoutMarker->addRow(tr("Young modulus (Pa):"), txtYoungModulus);
    layoutMarker->addRow(tr("Poisson number (-):"), txtPoissonNumber);

    return layoutMarker;
}

void DSceneLabelElasticityMarker::load()
{
    DSceneLabelMarker::load();

    SceneLabelElasticityMarker *labelElasticityMarker = dynamic_cast<SceneLabelElasticityMarker *>(m_labelMarker);

    txtYoungModulus->setText(QString::number(labelElasticityMarker->young_modulus));
    txtPoissonNumber->setText(QString::number(labelElasticityMarker->poisson_ratio));
}

bool DSceneLabelElasticityMarker::save()
{
    if (!DSceneLabelMarker::save()) return false;;

    SceneLabelElasticityMarker *labelElasticityMarker = dynamic_cast<SceneLabelElasticityMarker *>(m_labelMarker);

    labelElasticityMarker->young_modulus = txtYoungModulus->text().toDouble();
    labelElasticityMarker->poisson_ratio = txtPoissonNumber->text().toDouble();

    return true;
}

//

QString htmlLabelLabelsFactory(PhysicField physicField)
{
    QString out;
    switch (physicField)
    {
    case PHYSICFIELD_ELECTROSTATIC:
        out = "<tr><td>&nbsp;</td><td>Charge density (C/m3)</td><td>Permittivity (-)</td></tr>";
        break;
    case PHYSICFIELD_MAGNETOSTATIC:
        out = "<tr><td>&nbsp;</td><td>Current density (A/m2)</td><td>Permeability (-)</td></tr>";
        break;
    case PHYSICFIELD_HARMONIC_MAGNETIC:
        out = "<tr><th>&nbsp;</th><th>Current density - real (A/m2)</th><th>Current density - imag (A/m2)</th><th>Permeability (-)</th><th>Conductivity (S/m2)</th></tr>";
        break;
    case PHYSICFIELD_CURRENT:
        out = "<tr><td>&nbsp;</td><td>Conductivity (S/m)</td></tr>";
        break;
    case PHYSICFIELD_HEAT_TRANSFER:
        out = "<tr><td>&nbsp;</td><td>Volume heat (W/m3)</td><td>Thermal conductivity (W/m.K)</td></tr>";
        break;
    case PHYSICFIELD_ELASTICITY:
        out = "<tr><td>&nbsp;</td><td>Young modulus (Pa)</td><td>Poisson ratio (-)</td></tr>";
        break;
    default:
        cerr << "Physical field '" + physicFieldStringKey(physicField).toStdString() + "' is not implemented. htmlLabelLabelsFactory(PhysicField physicField)" << endl;
        throw;
        break;
    }

    return QString(out);
}

QString htmlEdgeLabelsFactory(PhysicField physicField)
{
    QString out;
    switch (physicField)
    {
    case PHYSICFIELD_ELECTROSTATIC:
        out = "<tr><td>&nbsp;</td><td>Type</td><td>Value</td></tr>";
        break;
    case PHYSICFIELD_MAGNETOSTATIC:
        out = "<tr><td>&nbsp;</td><td>Type</td><td>Value</td></tr>";
        break;
    case PHYSICFIELD_HARMONIC_MAGNETIC:
        out = "<tr><td>&nbsp;</td><td>Type</td><td>Value</td></tr>";
        break;
    case PHYSICFIELD_CURRENT:
        out = "<tr><td>&nbsp;</td><td>Type</td><td>Value</td></tr>";
        break;
    case PHYSICFIELD_HEAT_TRANSFER:
        out = "<tr><td>&nbsp;</td><td>Type</td><td>Value</td></tr>";
        break;
    case PHYSICFIELD_ELASTICITY:
        out = "<tr><td>&nbsp;</td><td>Type X</td><td>Value X</td><td>Type Y</td><td>Value Y</td></tr>";
        break;
    default:
        cerr << "Physical field '" + physicFieldStringKey(physicField).toStdString() + "' is not implemented. htmlEdgeLabelsFactory(PhysicField physicField)" << endl;
        throw;
        break;
    }

    return QString(out);
}


