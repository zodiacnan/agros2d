// This file is part of Agros2D.
//
// Agros2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Agros2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Agros2D.  If not, see <http://www.gnu.org/licenses/>.
//
// hp-FEM group (http://hpfem.org/)
// University of Nevada, Reno (UNR) and University of West Bohemia, Pilsen
// Email: agros2d@googlegroups.com, home page: http://hpfem.org/agros2d/

#ifdef WIN32
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include "GL/glew.h"
#endif
#endif

#include <deal.II/numerics/data_out.h>
#include <deal.II/numerics/data_postprocessor.h>
#include <deal.II/grid/filtered_iterator.h>

#include "sceneview_post.h"

#include "util/global.h"

#include "sceneview_common2d.h"
#include "sceneview_data.h"
#include "scene.h"
#include "scenemarker.h"
#include "scenemarkerdialog.h"
#include "scenemarkerselectdialog.h"
#include "scenebasicselectdialog.h"

#include "scenebasic.h"
#include "scenenode.h"
#include "sceneedge.h"
#include "scenelabel.h"

#include "logview.h"

#include "hermes2d/plugin_interface.h"
#include "hermes2d/module.h"
#include "hermes2d/field.h"
#include "hermes2d/problem.h"
#include "hermes2d/problem_config.h"
#include "hermes2d/solutiontypes.h"
#include "hermes2d/solutionstore.h"

#include "pythonlab/pythonengine.h"

PostDataOut::PostDataOut() : dealii::DataOut<2>()
{
}

void PostDataOut::compute_nodes(QList<PostTriangle> &values)
{
    values.clear();

    m_min =  numeric_limits<double>::max();
    m_max = -numeric_limits<double>::max();

    dealii::Point<2> node0, node1, node2, node3;

    // loop over all patches
    for (typename std::vector<dealii::DataOutBase::Patch<2> >::const_iterator patch = patches.begin(); patch != patches.end(); ++patch)
    {
        const unsigned int n_subdivisions = patch->n_subdivisions;
        const unsigned int n = n_subdivisions + 1;
        unsigned int d1 = 1;
        unsigned int d2 = n;

        for (unsigned int i2=0; i2<n-1; ++i2)
        {
            for (unsigned int i1=0; i1<n-1; ++i1)
            {
                // compute coordinates for this patch point
                compute_node(node0, &*patch, i1, i2, 0, n_subdivisions);
                compute_node(node1, &*patch, i1, i2+1, 0, n_subdivisions);
                compute_node(node2, &*patch, i1+1, i2, 0, n_subdivisions);
                compute_node(node3, &*patch, i1+1, i2+1, 0, n_subdivisions);

                double value0 = patch->data(0, i1*d1 + i2*d2);
                double value1 = patch->data(0, i1*d1 + (i2+1)*d2);
                double value2 = patch->data(0, (i1+1)*d1 + i2*d2);
                double value3 = patch->data(0, (i1+1)*d1 + (i2+1)*d2);

                m_min = std::min(std::min(std::min(std::min(m_min, value0), value1), value2), value3);
                m_max = std::max(std::max(std::max(std::max(m_max, value0), value1), value2), value3);

                values.append(PostTriangle(node0, node1, node2, value0, value1, value2));
                values.append(PostTriangle(node1, node3, node2, value1, value3, value2));
            }
        }
    }
}

void PostDataOut::compute_node(dealii::Point<2> &node, const dealii::DataOutBase::Patch<2> *patch,
                               const unsigned int xstep, const unsigned int ystep, const unsigned int zstep,
                               const unsigned int n_subdivisions)
{
    if (patch->points_are_available)
    {
        unsigned int point_no=0;
        // note: switch without break !
        Assert (ystep<n_subdivisions+1, ExcIndexRange(ystep,0,n_subdivisions+1));
        point_no+=(n_subdivisions+1)*ystep;
        for (unsigned int d=0; d<2; ++d)
            node[d]=patch->data(patch->data.size(0)-2+d,point_no);
    }
    else
    {
        // perform a dim-linear interpolation
        const double stepsize=1./n_subdivisions, xfrac=xstep*stepsize;

        node = (patch->vertices[1] * xfrac) + (patch->vertices[0] * (1-xfrac));
        const double yfrac=ystep*stepsize;
        node*= 1-yfrac;
        node += ((patch->vertices[3] * xfrac) + (patch->vertices[2] * (1-xfrac))) * yfrac;
    }
}


typename dealii::DataOut<2>::cell_iterator PostDataOut::first_cell()
{
    if (m_subdomains.size() == 0)
        return this->dofs->begin_active();

    typename DataOut<2>::active_cell_iterator
            cell = this->dofs->begin_active();
    while ((cell != this->dofs->end()) &&
           (cell->subdomain_id() != m_subdomains[0])) // TODO !!!
        ++cell;

    return cell;
}

typename dealii::DataOut<2>::cell_iterator PostDataOut::next_cell (const typename DataOut<2>::cell_iterator &old_cell)
{
    if (m_subdomains.size() == 0)
        return dealii::DataOut<2>::next_cell(old_cell);

    if (old_cell != this->dofs->end())
    {
        const dealii::IteratorFilters::SubdomainEqualTo predicate(m_subdomains[0]); // TODO !!!!
        return ++(dealii::FilteredIterator <typename dealii::DataOut<2>::active_cell_iterator>(predicate, old_cell));
    }
    else
    {
        return old_cell;
    }
}

// ************************************************************************************************************************

PostDeal::PostDeal() :
    m_activeViewField(NULL),
    m_activeTimeStep(NOT_FOUND_SO_FAR),
    m_activeAdaptivityStep(NOT_FOUND_SO_FAR),
    m_activeSolutionMode(SolutionMode_Undefined),
    m_isProcessed(false)
{
    connect(Agros2D::scene(), SIGNAL(cleared()), this, SLOT(clear()));
    connect(Agros2D::problem(), SIGNAL(clearedSolution()), this, SLOT(clearView()));
    connect(Agros2D::problem(), SIGNAL(fieldsChanged()), this, SLOT(clear()));

    connect(Agros2D::problem(), SIGNAL(meshed()), this, SLOT(problemMeshed()));
    connect(Agros2D::problem(), SIGNAL(solved()), this, SLOT(problemSolved()));
}

PostDeal::~PostDeal()
{
    clear();
}

void PostDeal::processRangeContour()
{
    if (Agros2D::problem()->isSolved() && m_activeViewField && (Agros2D::problem()->setting()->value(ProblemSetting::View_ShowContourView).toBool()))
    {
        bool contains = false;
        foreach (Module::LocalVariable variable, m_activeViewField->viewScalarVariables())
        {
            if (variable.id() == Agros2D::problem()->setting()->value(ProblemSetting::View_ContourVariable).toString())
            {
                contains = true;
                break;
            }
        }

        Agros2D::log()->printMessage(tr("Post View"), tr("Contour view (%1)").arg(Agros2D::problem()->setting()->value(ProblemSetting::View_ContourVariable).toString()));

        QString variableName = Agros2D::problem()->setting()->value(ProblemSetting::View_ContourVariable).toString();
        Module::LocalVariable variable = m_activeViewField->localVariable(variableName);

        m_contourValues.clear();

        std::shared_ptr<PostDataOut> data_out;

        if (variable.isScalar())
            data_out = viewScalarFilter(m_activeViewField->localVariable(Agros2D::problem()->setting()->value(ProblemSetting::View_ContourVariable).toString()),
                                        PhysicFieldVariableComp_Scalar);

        else
            data_out = viewScalarFilter(m_activeViewField->localVariable(Agros2D::problem()->setting()->value(ProblemSetting::View_ContourVariable).toString()),
                                        PhysicFieldVariableComp_Magnitude);

        data_out->compute_nodes(m_contourValues);

        // deformed shape
        /*
        if (m_activeViewField->hasDeformableShape() && Agros2D::problem()->setting()->value(ProblemSetting::View_DeformContour).toBool())
        {
            Hermes::Hermes2D::MagFilter<double> *filter
                    = new Hermes::Hermes2D::MagFilter<double>({activeMultiSolutionArray().solutions().at(0), activeMultiSolutionArray().solutions().at(1)});

            if (fabs(filter->get_approx_max_value() - filter->get_approx_min_value()) > EPS_ZERO)
            {
                RectPoint rect = Agros2D::scene()->boundingBox();
                double dmult = qMax(rect.width(), rect.height()) / filter->get_approx_max_value() / 15.0;

                m_linContourView->set_displacement(activeMultiSolutionArray().solutions().at(0),
                                                   activeMultiSolutionArray().solutions().at(1),
                                                   dmult);
            }
            delete filter;
        }
        else
        {
            m_linContourView->set_displacement(NULL, NULL);
        }
        */
    }
}

void PostDeal::processRangeScalar()
{
    if ((Agros2D::problem()->isSolved()) && (m_activeViewField)
            && ((Agros2D::problem()->setting()->value(ProblemSetting::View_ShowScalarView).toBool())
                || (((SceneViewPost3DMode) Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarView3DMode).toInt()) == SceneViewPost3DMode_ScalarView3D)))
    {
        bool contains = false;
        foreach (Module::LocalVariable variable, m_activeViewField->viewScalarVariables())
        {
            if (variable.id() == Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarVariable).toString())
            {
                contains = true;
                break;
            }
        }

        Agros2D::log()->printMessage(tr("Post View"), tr("Scalar view (%1)").arg(Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarVariable).toString()));

        std::shared_ptr<PostDataOut> data_out = viewScalarFilter(m_activeViewField->localVariable(Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarVariable).toString()),
                                                                 (PhysicFieldVariableComp) Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarVariableComp).toInt());
        data_out->compute_nodes(m_scalarValues);

        /*
        // deformed shape
        if (m_activeViewField->hasDeformableShape() && Agros2D::problem()->setting()->value(ProblemSetting::View_DeformScalar).toBool())
        {
            Hermes::Hermes2D::MagFilter<double> *filter
                    = new Hermes::Hermes2D::MagFilter<double>({activeMultiSolutionArray().solutions().at(0), activeMultiSolutionArray().solutions().at(1)});

            if (fabs(filter->get_approx_max_value() - filter->get_approx_min_value()) > EPS_ZERO)
            {
                RectPoint rect = Agros2D::scene()->boundingBox();
                double dmult = qMax(rect.width(), rect.height()) / filter->get_approx_max_value() / 15.0;

                m_linScalarView->set_displacement(activeMultiSolutionArray().solutions().at(0),
                                                  activeMultiSolutionArray().solutions().at(1),
                                                  dmult);
            }
            delete filter;
        }
        else
        {
            m_linScalarView->set_displacement(NULL, NULL);
        }
        */
        if (Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarRangeAuto).toBool())
        {
            Agros2D::problem()->setting()->setValue(ProblemSetting::View_ScalarRangeMin, data_out->min());
            Agros2D::problem()->setting()->setValue(ProblemSetting::View_ScalarRangeMax, data_out->max());
        }
    }
}

void PostDeal::processRangeVector()
{
    if ((Agros2D::problem()->isSolved()) && (m_activeViewField) && (Agros2D::problem()->setting()->value(ProblemSetting::View_ShowVectorView).toBool()))
    {
        bool contains = false;
        foreach (Module::LocalVariable variable, m_activeViewField->viewVectorVariables())
        {
            if (variable.id() == Agros2D::problem()->setting()->value(ProblemSetting::View_VectorVariable).toString())
            {
                contains = true;
                break;
            }
        }

        Agros2D::log()->printMessage(tr("Post View"), tr("Vector view (%1)").arg(Agros2D::problem()->setting()->value(ProblemSetting::View_VectorVariable).toString()));

        std::shared_ptr<PostDataOut> data_outX = viewScalarFilter(m_activeViewField->localVariable(Agros2D::problem()->setting()->value(ProblemSetting::View_VectorVariable).toString()),
                                                                  PhysicFieldVariableComp_X);

        std::shared_ptr<PostDataOut> data_outY = viewScalarFilter(m_activeViewField->localVariable(Agros2D::problem()->setting()->value(ProblemSetting::View_VectorVariable).toString()),
                                                                  PhysicFieldVariableComp_Y);

        data_outX->compute_nodes(m_vectorXValues);
        data_outY->compute_nodes(m_vectorYValues);

        /*
        // deformed shape
        if (m_activeViewField->hasDeformableShape() && Agros2D::problem()->setting()->value(ProblemSetting::View_DeformVector).toBool())
        {
            Hermes::Hermes2D::MagFilter<double> *filter
                    = new Hermes::Hermes2D::MagFilter<double>({ activeMultiSolutionArray().solutions().at(0), activeMultiSolutionArray().solutions().at(1)});
            if (fabs(filter->get_approx_max_value() - filter->get_approx_min_value()) > EPS_ZERO)
            {
                RectPoint rect = Agros2D::scene()->boundingBox();
                double dmult = qMax(rect.width(), rect.height()) / filter->get_approx_max_value() / 15.0;

                m_vecVectorView->set_displacement(activeMultiSolutionArray().solutions().at(0),
                                                  activeMultiSolutionArray().solutions().at(1),
                                                  dmult);
            }
            delete filter;
        }
        else
        {
            m_vecVectorView->set_displacement(NULL, NULL);
        }
        */
    }
}

void PostDeal::clearView()
{
    m_isProcessed = false;

    m_contourValues.clear();
    m_scalarValues.clear();
    m_vectorXValues.clear();
    m_vectorYValues.clear();
}

void PostDeal::refresh()
{
    Agros2D::problem()->setIsPostprocessingRunning();
    clearView();

    if (Agros2D::problem()->isSolved())
        processSolved();

    m_isProcessed = true;
    emit processed();
    Agros2D::problem()->setIsPostprocessingRunning(false);
}

void PostDeal::clear()
{
    clearView();

    m_activeViewField = NULL;
    m_activeTimeStep = NOT_FOUND_SO_FAR;
    m_activeAdaptivityStep = NOT_FOUND_SO_FAR;
    m_activeSolutionMode = SolutionMode_Undefined;
}

void PostDeal::problemMeshed()
{
    if (!m_activeViewField)
    {
        setActiveViewField(Agros2D::problem()->fieldInfos().begin().value());
    }
    //    if (m_activeTimeStep == NOT_FOUND_SO_FAR)
    //    {
    //        setActiveTimeStep(0);
    //    }
    //    if (m_activeAdaptivityStep == NOT_FOUND_SO_FAR)
    //    {
    //        setActiveAdaptivityStep(0);
    //        setActiveAdaptivitySolutionType(SolutionMode_Normal);
    //    }
}

void PostDeal::problemSolved()
{
    if (!m_activeViewField)
    {
        setActiveViewField(Agros2D::problem()->fieldInfos().begin().value());
    }

    // time step
    int lastTimeStep = Agros2D::solutionStore()->lastTimeStep(m_activeViewField, SolutionMode_Normal);

    if (m_activeTimeStep == NOT_FOUND_SO_FAR || activeTimeStep() > lastTimeStep)
    {
        setActiveTimeStep(lastTimeStep);
    }

    // adaptive step
    int lastAdaptivityStep = Agros2D::solutionStore()->lastAdaptiveStep(m_activeViewField, SolutionMode_Normal, m_activeTimeStep);

    setActiveAdaptivityStep(lastAdaptivityStep);
    setActiveAdaptivitySolutionType(SolutionMode_Normal);
}

void PostDeal::processSolved()
{
    // update time functions
    if (Agros2D::problem()->isTransient())
        Module::updateTimeFunctions(Agros2D::problem()->timeStepToTotalTime(activeTimeStep()));

    FieldSolutionID fsid(activeViewField(), activeTimeStep(), activeAdaptivityStep(), activeAdaptivitySolutionType());
    if (Agros2D::solutionStore()->contains(fsid))
    {
        /*
        MultiArray<double> ma = Agros2D::solutionStore()->multiArray(fsid);
        if (ma.spaces().empty())
            return;
        if (ma.solutions().empty())
            return;
        */

        // add icon to progress
        Agros2D::log()->addIcon(icon("scene-post2d"), tr("Postprocessor"));

        processRangeContour();
        processRangeScalar();
        processRangeVector();
    }
}


std::shared_ptr<PostDataOut> PostDeal::viewScalarFilter(Module::LocalVariable physicFieldVariable,
                                                        PhysicFieldVariableComp physicFieldVariableComp)
{
    qDebug() << activeViewField()->fieldId() << activeTimeStep() << activeAdaptivityStep() << activeAdaptivitySolutionType();

    // update time functions
    if (Agros2D::problem()->isTransient())
        Module::updateTimeFunctions(Agros2D::problem()->timeStepToTotalTime(activeTimeStep()));

    MultiArrayDeal ma = activeMultiSolutionArray();

    std::shared_ptr<dealii::DataPostprocessorScalar<2> > post = activeViewField()->plugin()->filter(activeViewField(),
                                                                                                    activeTimeStep(),
                                                                                                    activeAdaptivityStep(),
                                                                                                    activeAdaptivitySolutionType(),
                                                                                                    &ma,
                                                                                                    physicFieldVariable.id(),
                                                                                                    physicFieldVariableComp);

    qDebug() << "OK 1";
    std::shared_ptr<PostDataOut> data_out = std::shared_ptr<PostDataOut>(new PostDataOut());
    qDebug() << "OK 2" << ma.doFHandler()->n_dofs();
    data_out->attach_dof_handler(*ma.doFHandler());
    qDebug() << "OK 3";
    data_out->add_data_vector(*ma.solution(), "solution");
    // data_out->add_data_vector(*ma.solution(), *post);
    data_out->build_patches(2);
    qDebug() << "OK 4";

    return data_out;
}

void PostDeal::setActiveViewField(FieldInfo* fieldInfo)
{
    // previous active field
    FieldInfo* previousActiveViewField = m_activeViewField;

    // set new field
    m_activeViewField = fieldInfo;

    // check for different field
    if (previousActiveViewField != fieldInfo)
    {
        setActiveTimeStep(NOT_FOUND_SO_FAR);
        setActiveAdaptivityStep(NOT_FOUND_SO_FAR);
        setActiveAdaptivitySolutionType(SolutionMode_Normal);

        // set default variables
        Module::LocalVariable scalarVariable = m_activeViewField->defaultViewScalarVariable();
        Module::LocalVariable vectorVariable = m_activeViewField->defaultViewVectorVariable();

        QString scalarVariableDefault = scalarVariable.id();
        PhysicFieldVariableComp scalarVariableCompDefault = scalarVariable.isScalar() ? PhysicFieldVariableComp_Scalar : PhysicFieldVariableComp_Magnitude;
        QString contourVariableDefault = scalarVariable.id();
        QString vectorVariableDefault = vectorVariable.id();

        foreach (Module::LocalVariable local, m_activeViewField->viewScalarVariables())
        {
            if (Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarVariable).toString() == local.id())
            {
                scalarVariableDefault = Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarVariable).toString();
                scalarVariableCompDefault = (PhysicFieldVariableComp) Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarVariableComp).toInt();
            }
            if (Agros2D::problem()->setting()->value(ProblemSetting::View_ContourVariable).toString() == local.id())
            {
                contourVariableDefault = Agros2D::problem()->setting()->value(ProblemSetting::View_ContourVariable).toString();
            }
        }
        foreach (Module::LocalVariable local, m_activeViewField->viewScalarVariables())
        {
            if (Agros2D::problem()->setting()->value(ProblemSetting::View_VectorVariable).toString() == local.id())
            {
                vectorVariableDefault = Agros2D::problem()->setting()->value(ProblemSetting::View_VectorVariable).toString();
            }
        }

        Agros2D::problem()->setting()->setValue(ProblemSetting::View_ScalarVariable, scalarVariableDefault);
        Agros2D::problem()->setting()->setValue(ProblemSetting::View_ScalarVariableComp, scalarVariableCompDefault);
        Agros2D::problem()->setting()->setValue(ProblemSetting::View_ContourVariable, contourVariableDefault);
        Agros2D::problem()->setting()->setValue(ProblemSetting::View_VectorVariable, vectorVariableDefault);

        // order component
        Agros2D::problem()->setting()->setValue(ProblemSetting::View_OrderComponent, 1);
    }
}

void PostDeal::setActiveTimeStep(int ts)
{
    m_activeTimeStep = ts;
    Agros2D::problem()->setActualTimePostprocessing(Agros2D::problem()->timeStepToTime(ts));
}

void PostDeal::setActiveAdaptivityStep(int as)
{
    m_activeAdaptivityStep = as;
}

MultiArrayDeal PostDeal::activeMultiSolutionArray()
{
    FieldSolutionID fsid(activeViewField(), activeTimeStep(), activeAdaptivityStep(), activeAdaptivitySolutionType());
    return Agros2D::solutionStore()->multiArrayDeal(fsid);
}

// ************************************************************************************************

SceneViewPostInterface::SceneViewPostInterface(PostDeal *postDeal, QWidget *parent)
    : SceneViewCommon(parent),
      m_postDeal(postDeal),
      m_textureScalar(0)
{
}

void SceneViewPostInterface::initializeGL()
{
    clearGLLists();

    SceneViewCommon::initializeGL();
}

const double* SceneViewPostInterface::paletteColor(double x) const
{
    switch ((PaletteType) Agros2D::problem()->setting()->value(ProblemSetting::View_PaletteType).toInt())
    {
    case Palette_Agros2D:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataAgros2D[n];
    }
        break;
    case Palette_Jet:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataJet[n];
    }
        break;
    case Palette_Copper:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataCopper[n];
    }
        break;
    case Palette_Hot:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataHot[n];
    }
        break;
    case Palette_Cool:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataCool[n];
    }
        break;
    case Palette_Bone:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataBone[n];
    }
        break;
    case Palette_Pink:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataPink[n];
    }
        break;
    case Palette_Spring:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataSpring[n];
    }
        break;
    case Palette_Summer:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataSummer[n];
    }
        break;
    case Palette_Autumn:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataAutumn[n];
    }
        break;
    case Palette_Winter:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataWinter[n];
    }
        break;
    case Palette_HSV:
    {
        if (x < 0.0) x = 0.0;
        else if (x > 1.0) x = 1.0;
        x *= PALETTEENTRIES;
        int n = (int) x;
        return paletteDataHSV[n];
    }
        break;
    case Palette_BWAsc:
    {
        static double color[3];
        color[0] = color[1] = color[2] = x;
        return color;
    }
        break;
    case Palette_BWDesc:
    {
        static double color[3];
        color[0] = color[1] = color[2] = 1.0 - x;
        return color;
    }
        break;
    default:
        qWarning() << QString("Undefined: %1.").arg(((PaletteType) Agros2D::problem()->setting()->value(ProblemSetting::View_PaletteType).toInt()));
        return NULL;
    }
}

const double* SceneViewPostInterface::paletteColorOrder(int n) const
{
    switch ((PaletteOrderType) Agros2D::problem()->setting()->value(ProblemSetting::View_OrderPaletteOrderType).toInt())
    {
    case PaletteOrder_Hermes:
        return paletteOrderHermes[n];
    case PaletteOrder_Jet:
        return paletteOrderJet[n];
    case PaletteOrder_Copper:
        return paletteOrderCopper[n];
    case PaletteOrder_Hot:
        return paletteOrderHot[n];
    case PaletteOrder_Cool:
        return paletteOrderCool[n];
    case PaletteOrder_Bone:
        return paletteOrderBone[n];
    case PaletteOrder_Pink:
        return paletteOrderPink[n];
    case PaletteOrder_Spring:
        return paletteOrderSpring[n];
    case PaletteOrder_Summer:
        return paletteOrderSummer[n];
    case PaletteOrder_Autumn:
        return paletteOrderAutumn[n];
    case PaletteOrder_Winter:
        return paletteOrderWinter[n];
    case PaletteOrder_HSV:
        return paletteOrderHSV[n];
    case PaletteOrder_BWAsc:
        return paletteOrderBWAsc[n];
    case PaletteOrder_BWDesc:
        return paletteOrderBWDesc[n];
    default:
        qWarning() << QString("Undefined: %1.").arg(Agros2D::problem()->setting()->value(ProblemSetting::View_OrderPaletteOrderType).toInt());
        return NULL;
    }
}

void SceneViewPostInterface::paletteCreate()
{
    int paletteSteps = Agros2D::problem()->setting()->value(ProblemSetting::View_PaletteFilter).toBool()
            ? 100 : Agros2D::problem()->setting()->value(ProblemSetting::View_PaletteSteps).toInt();

    unsigned char palette[256][3];
    for (int i = 0; i < paletteSteps; i++)
    {
        const double* color = paletteColor((double) i / paletteSteps);
        palette[i][0] = (unsigned char) (color[0] * 255);
        palette[i][1] = (unsigned char) (color[1] * 255);
        palette[i][2] = (unsigned char) (color[2] * 255);
    }
    for (int i = paletteSteps; i < 256; i++)
        memcpy(palette[i], palette[paletteSteps-1], 3);

    makeCurrent();
    if (glIsTexture(m_textureScalar))
        glDeleteTextures(1, &m_textureScalar);
    glGenTextures(1, &m_textureScalar);

    glBindTexture(GL_TEXTURE_1D, m_textureScalar);
#ifdef Q_WS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) // This somehow does not work for Qt 4.8 in Windows.
    glTexParameteri(GL_TEXTURE_1D, GL_GENERATE_MIPMAP, GL_TRUE);
#endif
#else
    glTexParameteri(GL_TEXTURE_1D, GL_GENERATE_MIPMAP, GL_TRUE);
#endif
    if (Agros2D::problem()->setting()->value(ProblemSetting::View_PaletteFilter).toBool())
    {
#ifdef Q_WS_WIN
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#else
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#endif
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
#ifdef Q_WS_WIN
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#else
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
#endif
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    glTexImage1D(GL_TEXTURE_1D, 0, 3, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, palette);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);

    // adjust palette
    if (Agros2D::problem()->setting()->value(ProblemSetting::View_PaletteFilter).toBool())
    {
        m_texScale = (double) (paletteSteps-1) / 256.0;
        m_texShift = 0.5 / 256.0;
    }
    else
    {
        m_texScale = (double) paletteSteps / 256.0;
        m_texShift = 0.0;
    }
}

void SceneViewPostInterface::paintScalarFieldColorBar(double min, double max)
{
    if (!Agros2D::problem()->isSolved() || !Agros2D::problem()->setting()->value(ProblemSetting::View_ShowScalarColorBar).toBool()) return;

    loadProjectionViewPort();

    glScaled(2.0 / width(), 2.0 / height(), 1.0);
    glTranslated(-width() / 2.0, -height() / 2.0, 0.0);

    // dimensions
    int textWidth = (m_charDataPost[GLYPH_M].x1 - m_charDataPost[GLYPH_M].x0)
            * (QString::number(-1.0, 'e', Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarDecimalPlace).toInt()).length() + 1);
    int textHeight = 2 * (m_charDataPost[GLYPH_M].y1 - m_charDataPost[GLYPH_M].y0);
    Point scaleSize = Point(45.0 + textWidth, 20*textHeight); // height() - 20.0
    Point scaleBorder = Point(10.0, (Agros2D::configComputer()->value(Config::Config_ShowRulers).toBool()) ? 1.8 * textHeight : 10.0);
    double scaleLeft = (width() - (45.0 + textWidth));
    int numTicks = 11;

    // blended rectangle
    drawBlend(Point(scaleLeft, scaleBorder.y), Point(scaleLeft + scaleSize.x - scaleBorder.x, scaleBorder.y + scaleSize.y),
              0.91, 0.91, 0.91);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // palette border
    glColor3d(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2d(scaleLeft + 30.0, scaleBorder.y + scaleSize.y - 50.0);
    glVertex2d(scaleLeft + 10.0, scaleBorder.y + scaleSize.y - 50.0);
    glVertex2d(scaleLeft + 10.0, scaleBorder.y + 10.0);
    glVertex2d(scaleLeft + 30.0, scaleBorder.y + 10.0);
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);

    // palette
    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, m_textureScalar);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glBegin(GL_QUADS);
    if (fabs(min - max) > EPS_ZERO)
        glTexCoord1d(m_texScale + m_texShift);
    else
        glTexCoord1d(m_texShift);
    glVertex2d(scaleLeft + 28.0, scaleBorder.y + scaleSize.y - 52.0);
    glVertex2d(scaleLeft + 12.0, scaleBorder.y + scaleSize.y - 52.0);
    glTexCoord1d(m_texShift);
    glVertex2d(scaleLeft + 12.0, scaleBorder.y + 12.0);
    glVertex2d(scaleLeft + 28.0, scaleBorder.y + 12.0);
    glEnd();

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDisable(GL_TEXTURE_1D);

    // ticks
    glLineWidth(1.0);
    glBegin(GL_LINES);
    for (int i = 1; i < numTicks; i++)
    {
        double tickY = (scaleSize.y - 60.0) / (numTicks - 1.0);

        glVertex2d(scaleLeft + 10.0, scaleBorder.y + scaleSize.y - 49.0 - i*tickY);
        glVertex2d(scaleLeft + 15.0, scaleBorder.y + scaleSize.y - 49.0 - i*tickY);
        glVertex2d(scaleLeft + 25.0, scaleBorder.y + scaleSize.y - 49.0 - i*tickY);
        glVertex2d(scaleLeft + 30.0, scaleBorder.y + scaleSize.y - 49.0 - i*tickY);
    }
    glEnd();

    // line
    glLineWidth(1.0);
    glBegin(GL_LINES);
    glVertex2d(scaleLeft + 5.0, scaleBorder.y + scaleSize.y - 31.0);
    glVertex2d(scaleLeft + scaleSize.x - 15.0, scaleBorder.y + scaleSize.y - 31.0);
    glEnd();

    // labels
    for (int i = 1; i < numTicks+1; i++)
    {
        double value = 0.0;
        if (!Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarRangeLog).toBool())
            value = min + (double) (i-1) / (numTicks-1) * (max - min);
        else
            value = min + (double) pow((double) Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarRangeBase).toInt(),
                                       ((i-1) / (numTicks-1)))/Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarRangeBase).toInt() * (max - min);

        if (fabs(value) < EPS_ZERO) value = 0.0;
        double tickY = (scaleSize.y - 60.0) / (numTicks - 1.0);

        printPostAt(scaleLeft + 33.0 + ((value >= 0.0) ? (m_charDataPost[GLYPH_M].x1 - m_charDataPost[GLYPH_M].x0) : 0.0),
                    scaleBorder.y + 10.0 + (i-1)*tickY - textHeight / 4.0,
                    QString::number(value, 'e', Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarDecimalPlace).toInt()));
    }

    // variable
    Module::LocalVariable localVariable = postDeal()->activeViewField()->localVariable(Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarVariable).toString());
    QString str = QString("%1 (%2)").
            arg(Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarVariable).toString().isEmpty() ? "" : localVariable.shortname()).
            arg(Agros2D::problem()->setting()->value(ProblemSetting::View_ScalarVariable).toString().isEmpty() ? "" : localVariable.unit());

    printPostAt(scaleLeft + scaleSize.x / 2.0 - (m_charDataPost[GLYPH_M].x1 - m_charDataPost[GLYPH_M].x0) * str.count() / 2.0,
                scaleBorder.y + scaleSize.y - 20.0,
                str);
}
