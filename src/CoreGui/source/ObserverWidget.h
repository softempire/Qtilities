/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
**
** This file is part of Qtilities which is released under the following
** licensing options.
**
** Option 1: Open Source
** Under this license Qtilities is free software: you can
** redistribute it and/or modify it under the terms of the GNU General
** Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
**
** Qtilities is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Qtilities. If not, see http://www.gnu.org/licenses/.
**
** Option 2: Commercial
** Alternatively, this library is also released under a commercial license
** that allows the development of closed source proprietary applications
** without restrictions on licensing. For more information on this option,
** please see the project website's licensing page:
** http://www.qtilities.org/licensing.html
**
** If you are unsure which license is appropriate for your use, please
** contact support@qtilities.org.
**
****************************************************************************/

#ifndef OBSERVERWIDGET_H
#define OBSERVERWIDGET_H

#include "QtilitiesCoreGui_global.h"
#include "QtilitiesCoreGuiConstants.h"
#include "IActionProvider.h"
#include "ObjectPropertyBrowser.h"
#include "AbstractObserverTableModel.h"
#include "AbstractObserverTreeModel.h"
#include "SearchBoxWidget.h"

#include <Observer.h>
#include <ObserverHints.h>
#include <IContext.h>
#include <Observer.h>

#include <QMainWindow>
#include <QStack>
#include <QAbstractItemModel>
#include <QModelIndexList>
#include <QTableView>
#include <QTreeView>

namespace Ui {
    class ObserverWidget;
}

namespace Qtilities {
    namespace CoreGui {
        using namespace Qtilities::CoreGui::Interfaces;
        using namespace Qtilities::CoreGui::Constants;
        using namespace Qtilities::Core::Interfaces;
        using namespace Qtilities::Core;

        /*!
        \struct ObserverWidgetData
        \brief A structure used by ObserverWidget classes to store data.
          */
        class ObserverWidgetData;

        /*!
          \class ObserverWidget
          \brief The ObserverWidget class provides a ready-to-use widget to display information about a specific observer context.

          The ObserverWidget class provides a ready-to-use widget to display information about a specific observer context, reducing the
          workload when you need to display data related to a specific observer context. The goal of the observer widget is to maximize the possible
          number of usage scenarios by being as customizable as possible.

          Below is an example of a basic observer widget in tree mode.

          \image html observer_widget_tree.jpg "Observer Widget (Tree View Mode)"
          \image latex observer_widget_tree.eps "Observer Widget (Tree View Mode)" width=3in

          The \ref page_observer_widgets article provides a comprehensive overview of the different ways that the observer widget can be used.
          */
        class QTILITIES_CORE_GUI_SHARED_EXPORT ObserverWidget : public QMainWindow, public ObserverAwareBase, public IContext {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::IContext)
            Q_ENUMS(DisplayMode)

        public:
            // --------------------------------
            // Core Functions
            // --------------------------------
            //! Default constructor.
            /*!
              \param display_mode The display mode that should be used.
              \param parent The parent widget.
              \param f The Qt::WindowFlags which must be used for the widget.
              */
            ObserverWidget(DisplayMode display_mode = TreeView, QWidget * parent = 0, Qt::WindowFlags f = 0);
            //! Default destructor.
            virtual ~ObserverWidget();
            //! Implementation of virtual function ObserverAwareBase::setObserverContext().
            bool setObserverContext(Observer* observer);
            //! Initializes the observer widget. Make sure to set the item model as well as the flags you would like to use before calling initialize.
            virtual void initialize(bool hints_only = false);
            //! Gets the current navigation stack of this widget.
            QStack<int> navigationStack() const;
            //! Allows you to set the navigation stack of this widget.
            /*!
              The navigation stack keeps track of the user's navigation history of events related to pushing down into observers and
              pushing up into parents of observers. Using this function you can initialize the widget using a predefined stack. The
              stack stores IDs of observers assigned by the Qtilities::CoreGui::ObjectManager singleton.

              \sa
                - initialize()
              */
            void setNavigationStack(QStack<int> navigation_stack);
            //! Returns the observer ID of the top level observer in the widget. If the top level observer is not defined, -1 is returned.
            int topLevelObserverID();
            //! Event filter which has responsibilities such as drag and drop operations etc.
            bool eventFilter(QObject *object, QEvent *event);
            //! Function which sets a custom table model to be used in this widget when its in TableView mode.
            /*!
              By default the observer widget uses the Qtilities::CoreGui::ObserverTableModel as the model for the table view.
              It is possible to create custom models which allows you to extend the default table model.

              \note This function must be called before initializing the widget for the first time.

              \returns True if the model was successfully set.
              */
            bool setCustomTableModel(AbstractObserverTableModel* table_model);
            //! Function which sets a custom tree model to be used in this widget when its in TreeView mode.
            /*!
              By default the observer widget uses the Qtilities::CoreGui::ObserverTreeModel as the model for the tree view.
              It is possible to create custom models which allows you to extend the default tree model.

              \note This function must be called before initializing the widget for the first time.

              \returns True if the model was successfully set.
              */
            bool setCustomTreeModel(AbstractObserverTreeModel* tree_model);
            //! Sets the display mode of the widget.
            /*!
              \sa displayMode(), toggleDisplayMode()
              */
            void setDisplayMode(DisplayMode display_mode);
            //! Gets the display mode of the widget.
            /*!
              \sa setDisplayMode(), toggleDisplayMode()
              */
            DisplayMode displayMode() const;
        public slots:
            void contextDeleted();
            //! The context detach handler check if any observer in the current context's parent hierarchy is deleted. If so, contextDeleted() is called.
            void contextDetachHandler(Observer::SubjectChangeIndication indication, QList<QObject*> obj);
            //! Slot which will call the handleSearchStringChanged() slot with an empty QString as parameter.
            void resetProxyModel();
        signals:
            //! Signal which is emitted when the observer context of this widget changes.
            void observerContextChanged(Observer* new_context);

            // --------------------------------
            // IContext implementation
            // --------------------------------
        public:
            //! In the case of an ObserverWidget, the contextString() is the same as the globalMetaType().
            QString contextString() const { return globalMetaType(); }
            QString contextHelpId() const { return QString(); }

            // --------------------------------
            // Functions Related To Display Hints
            // --------------------------------
            //! Function to toggle usage of hints from the active parent observer. If not default hints will be used.
            void toggleUseObserverHints(bool toggle);
            //! This function will set the hints used for the current selection parent.
            /*!
              \sa toggleUseObserverHints()
              */
            void inheritDisplayHints(ObserverHints* display_hints);
        private:
            //! This function will provide the hints which should be used by this widget at any time.
            /*!
              \sa toggleUseObserverHints()
              */
            ObserverHints* activeHints() const;
            ObserverHints* activeHints();

            // --------------------------------
            // Settings, Global Meta Type and Action Provider Functions
            // --------------------------------
        public slots:
            //! Monitors the settings update request signal on the QtilitiesCoreGui instance.
            void handleSettingsUpdateRequest(const QString& request_id);
            //! Slot connected to QCoreApplication::aboutToQuit() signal.
            /*!
              Saves settings about this ObserverWidget instance using QSettings. The following parameters are saved:
              - The main window state of the observer widget (ObserverWidget inherits QMainWindow).
              - The grid style. \sa toggleGrid()
              - The default table view row size (Only in TableView mode). \sa setDefaultRowHeight()
              - The display mode. \sa DisplayMode

              \note This connection is made in the readSettings() functions. Thus if you don't want to store settings
              for an ObserverWidget, don't read it when the widget is created.

              \sa readSettings(), globalMetaType()
              */
            virtual void writeSettings();
        public:
            //! Restores the widget to a previous state.
            /*!
              \note This function must be called only after initialize() was called.

              \sa writeSettings(), globalMetaType()
              */
            virtual void readSettings();
            //! Function which allows this observer widget to share global object activity with other observer widgets.
            /*!
              This function will allow this observer widget to share global object activity with other observer widgets.
              Because the globalMetaType() for each observer widget must be unique, it is required to use a shared meta type
              for cases where global object activity needs to be shared between multiple observer widgets.

              When using a shared global activity meta type, the normal globalMetaType() will be used for all the normal
              usage scenarios listed in the globalMetaType() documentation, except for the meta type used to identify a set of active
              objects in the object manager.

              \sa sharedGlobalMetaType(), globalMetaType(), setGlobalMetaType(), updateGlobalActiveSubjects()
              */
            void setSharedGlobalMetaType(const QString& shared_meta_type);
            //! Function to get the shared global activity meta type of this observer widget.
            /*!
              \returns The shared global activity meta type. If this feature is not used, QString() will be returned.

              \sa sharedGlobalMetaType(), globalMetaType(), setGlobalMetaType(), updateGlobalActiveSubjects()
              */
            QString sharedGlobalMetaType() const;
            //! Sets the global meta type used for this observer widget.
            /*!
              \returns True if the meta_type string was valid. The validity check is done by checking if that a context with the same name does not yet exist in the context manager.

              \sa globalMetaType(), sharedGlobalMetaType()
              */
            bool setGlobalMetaType(const QString& meta_type);
            //! Gets the global meta type used for this observer widget.
            /*!
              The global meta type is a string which defines this observer widget. The string must be a string which
              can be registered in the context manager. Thus, such a string must not yet exist as a context in the context
              manager.

              The global meta type is used for the following:
              - As the context which is used to register backends for any actions created by this widget.
              - During readSettings() and writeSettings() to uniquely define this widget.
              - As the meta type which is used to identify a set of active objects in the object manager. For more information see Qtilities::Core::Interfaces::IObjectManager::metaTypeActiveObjects().
              - It is recommended to use the global meta type as the request ID when monitoring settings update requests. \sa handleSettingsUpdateRequest()

              \returns The meta type used for this observer widget.

              \sa setGlobalMetaType(), updateGlobalActiveSubjects(), setSharedGlobalMetaType(), sharedGlobalMetaType()
              */
            QString globalMetaType() const;
            //! Sets the global object subject type used by this observer widget.
            /*!
              If objects are selected, they are set as the active objects. If no objects are selected, the observer context
              is set as the active object.

              For more information see the \ref meta_type_object_management section of the \ref page_object_management article.
              */
            void updateGlobalActiveSubjects();
            //! Function to toggle if this observer widget updates global active objects under its globalMetaType() meta type.
            /*!
              For more information on global active objects, see the \ref meta_type_object_management section of the \ref page_object_management article.

              \sa useGlobalActiveObjects
              */
            void toggleUseGlobalActiveObjects(bool toggle);
            //! Indicates if this observer widget updates global active objects.
            /*!
              \sa toggleUseGlobalActiveObjects();
              */
            bool useGlobalActiveObjects() const;

            // --------------------------------
            // Functions Related To Selected Objects & Refreshing Of The Views
            // --------------------------------
            //! Provides a list of QObject pointers to all the selected objects.
            QList<QObject*> selectedObjects() const;
            //! Provides a pointer to the current selection's parent. If no objects are selected, 0 is returned.
            Observer* selectionParent() const;
            //! Provides a list of QModelIndexes which are currently selected. Use this call instead of the item model selection's selectedIndexes() call since this function will map the indexes from the proxy model's indexes to the real model's indexes.
            QModelIndexList selectedIndexes() const;
            //! Function to set the default height used for the table view when this widget is used in TableView mode.
            /*!
              The default is 17.

              \sa defaultRowHeight()
              */
            void setDefaultRowHeight(int height);
            //! Function to get the default height used for the table view when this widget is used in TableView mode.
            /*!
              \sa setDefaultRowHeight()
              */
            int defaultRowHeight() const;
            //! Function providing access to the contained table view.
            /*!
              \note This reference is only valid in TableView mode.
              */
            QTableView* const tableView();
            //! Function providing access to the contained tree view.
            /*!
              \note This reference is only valid in TreeView mode.
              */
            QTreeView* const treeView();
        private slots:
            //! Updates the current selection parent context.
            /*!
              This function is only used in TreeView mode.
              */
            void setTreeSelectionParent(Observer* observer);
        public slots:
            //! Selects the specified objects in the active item view.
            /*!
              \param objects The objects that must be selected. If any objects in the list are not present in the view, they will be ignored.

              \note This function does not respect the ObserverHints::ActivityControl::FollowSelection hint. You must do this manually.
              */
            void selectObjects(QList<QObject*> objects);
            //! Selects the specified objects in a smart pointer list in the active item view.
            /*!
              \param objects The objects that must be selected. If any objects in the list are not present in the view, they will be ignored.

              \note This function does not respect the ObserverHints::ActivityControl::FollowSelection hint. You must do this manually.
              */
            void selectObjects(QList<QPointer<QObject> > objects);
            //! Slot which resizes the rows in table view mode.
            /*!
              Slot which resizes the rows in table view mode.

              \param height The height which must be used. By default the default row heigth of 17 pixels is used.

              \sa defaultRowHeight(), setDefaultRowHeight()
              */
            void resizeTableViewRows(int height = -1);
            //! Handles the selection model change.
            /*!
              This function is called whenever the selection in the item view changes. The function
              will handle the selection change and then emit selectedObjectChanged().

              In TreeView mode, the function will call the Qtilities::CoreGui::ObserverTreeModel::calculateSelectionParent()
              function to update the selection parent in the tree. Once the selection parent is updated, the setTreeSelectionParent()
              function will be called which will call initialize(true) on this widget in order to initialize the
              widget for the new selection parent.
              */
            void handleSelectionModelChange();
        signals:
            //! Signal which is emitted when object selection changes.
            void selectedObjectsChanged(QList<QObject*> selected_objects, Observer* selection_parent = 0);

            // --------------------------------
            // Item Widgets Related Functions
            // --------------------------------
        public:
            #ifndef QTILITIES_NO_PROPERTY_BROWSER
            //! Returns the property editor used inside the observer widget. This can be 0 depending on the display flags used. Always call this function after initialize().
            ObjectPropertyBrowser* propertyBrowser();
            #endif
            //! Returns the SearchBoxWidget contained inside the observer widget.
            /*!
              \returns The search box widget instance, if it does not exist null is returned.
              */
            SearchBoxWidget* searchBoxWidget();

        public slots:
            #ifndef QTILITIES_NO_PROPERTY_BROWSER
            //! Sets the desired area of the property editor (if it is used by the observer context).
            /*!
              This area will be used to position the property editor dock widget when the widget is first shown during a session. Afterwards the widget will remember where the dock widget is.
              */
            void setPreferredPropertyEditorDockArea(Qt::DockWidgetArea property_editor_dock_area);
            //! Sets the desired type of the property editor (if it is used by the observer context).
            /*!
              The property editor type must be set before calling initialize().
              */
            void setPreferredPropertyEditorType(ObjectPropertyBrowser::BrowserType property_editor_type);
            //! Constructs the property browser and show it. If it already exists, this function does nothing.
            void constructPropertyBrowser();
        protected:
            //! Refreshes the property browser, thus hide or show it depending on the active display flags.
            void refreshPropertyBrowser();
            #endif
        protected:
            //! Refreshes the action toolbar with the current state of the actions.
            void refreshActionToolBar();
            //! Deletes all the current action toolbars.
            void deleteActionToolBars();

            // --------------------------------
            // Action Handlers and Related Functions
            // --------------------------------
        public:
            //! Returns the action handler interface for this observer widget.
            IActionProvider* actionProvider();
        public slots:
            //! Detaches the current selected objects in the item view from the current selection parent.
            /*!
              \sa selectionParent(), selectedObjects(), selectedObjectsChanged()
              */
            void selectionDetach();
            //! Detaches all subjects from the current selection parent.
            /*!
              \sa selectionParent(), selectedObjects(), selectedObjectsChanged()
              */
            void selectionDetachAll();
            //! Deletes the current selected items.
            /*!
              \sa selectedObjects(), selectedObjectsChanged()
              */
            void selectionDelete();
            //! Deletes all subjects under the current selection parent.
            /*!
              \sa selectionParent(), selectedObjects(), selectedObjectsChanged()
              */
            void selectionDeleteAll();
            //! This function is triggered by the Qtilities::Core::ObserverHints::ActionNewItem action.
            void handle_actionNewItem_triggered();
            //! Refreshes the current item view.
            /*!
              This function will emit the refreshViewsData() signal on the top level observer context.
              The refresh operation will be followed by calling selectObjects() with the same objects
              which were selected before calling this function was called. The function will then call
              refreshAction() before it exists.

              \sa topLevelObserverID(), addActionNewItem_triggered(), refreshActions();
              */
            void refresh();
            //! In TableView mode this function can be used to push up to (set the observer context of the widget) to the current selection parent.
            /*!
              \sa selectionParent(), selectionPushUpNew()
              */
            void selectionPushUp();
            //! In TableView mode this function can be used to push up to (set the observer context of the widget) to the current selection parent in a new ObserverWidget.
            /*!
              \sa selectionParent(), selectionPushUp()
              */
            void selectionPushUpNew();
            //! In TableView mode this function can be used to push down into (set the observer context of the widget) to the current selected observer.
            /*!
              This function only does something if an observer or an object which contains an observer is selected.

              \sa selectionParent(), selectionPushDownNew()
              */
            void selectionPushDown();
            //! In TableView mode this function can be used to push down into (set the observer context of the widget) to the current selected observer in a new ObserverWidget.
            /*!
              This function only does something if an observer or an object which contains an observer is selected.

              \sa selectionParent(), selectionPushDown()
              */
            void selectionPushDownNew();
            //! Toggles the display mode of the ObserverWidget.
            /*!
              \sa setDisplayMode(), displayMode()
              */
            void toggleDisplayMode();
            //! Function to copy the current selection to the application clipboard.
            void selectionCopy();
            //! Function to cut the current selection to the application clipboard.
            void selectionCut();
            //! Function which is connected to the Qtilities::CoreGui::Actions::MENU_EDIT_PASTE action if it exists.
            /*!
              When initializing the %Qtilities clipboard manager the paste action will be created automatically.
              */
            void handle_actionPaste_triggered();
            //! Toggles the visibility of the SearchBoxWidget at the bottom of the ObserverWidget.
            void toggleSearchBox();
            //! Collapse all items in the tree view to a depth of 1 in TreeView mode.
            void viewCollapseAll();
            //! Expand all items in the tree view in TreeView mode.
            void viewExpandAll();
            //! Handles search options changes in the SearchBoxWidget if present.
            void handleSearchOptionsChanged();
            //! Handles search string changes in the SearchBoxWidget if present.
            void handleSearchStringChanged(const QString& filter_string);
            //! Handle changes to the type of items which must be filtered.
            void handleSearchItemTypesChanged();
            //! Refreshes the state of all actions.
            void refreshActions();
        protected:
            //! Constructs actions inside the observer widget.
            void constructActions();

        private slots:
            //! This slot maps the point requested for a custom context menu from the item view's position to global position and then emitts customContextMenuRequested() with the correct point.
            void mapCustomContextMenuPoint(const QPoint & pos);

        signals:
            //! Signal which is emitted when the add new item action is triggered.
            /*!
              The parameters used during this signal emission is defferent depending on the display mode and the
              selected objects. The following are the possible scenarios:

              <b>1) Table View Mode: no object selected:</b>

              \param object The context displayed in the table.
              \param parent_observer The last context in the navigation stack, if no such context exists 0 is returned.

              <b>2) Table View Mode: 1 object selected:</b>

              If an observer is selected with the ObserverHints::SelectionUseSelectedContext hint the following parameters are used:
              \param object 0.
              \param parent_observer The selected observer.

              If an observer is selected with no hints or the ObserverHints::SelectionUseParentContext hint, or if an normal object is selected:
              \param object The selected object.
              \param parent_observer The context currently displayed in the table view.

              <b>3) Table View Mode: multiple objects selected:</b>

              \param object Null (0).
              \param parent_observer The context currently displayed in the table view.

              <b>4) Tree View Mode: no objects selected:</b>

              \param object Null (0).
              \param parent_observer The last context in the navigation stack, if no such context exists 0 is returned.

              <b>5) Tree View Mode: 1 object selected:</b>

              If an observer is selected with the ObserverHints::SelectionUseSelectedContext hint the following parameters are used:
              \param object 0.
              \param parent_observer The selected observer.

              If an observer is selected with no hints or the ObserverHints::SelectionUseParentContext hint, or if an normal object is selected:
              \param object The selected object.
              \param parent_observer The parent context of the selected object. If it does not have a parent, 0.

              <b>6) Tree View Mode: multiple objects selected:</b>

              \param object Null (0).
              \param parent_observer The parent context of the selected object. If it does not have a parent, 0.

              \note In Tree View Mode, categories are handled as normal QObjects with the category name accessible through the objectName() function.
              */
            void addActionNewItem_triggered(QObject* object, Observer* parent_observer = 0);
            //! Signal which is emitted when the user double clicks on an item in the observer widget.
            /*!
              The parameters used during this signal emission is defferent depending on the display mode and the
              selected objects. The following are the possible scenarios:

              <b>1) Table View Mode: 1 object selected:</b>

              If an observer is double clicked with the ObserverHints::SelectionUseSelectedContext hint the following parameters are used:
              \param object 0.
              \param parent_observer The selected observer.

              If an observer is double clicked with no hints or the ObserverHints::SelectionUseParentContext hint, or if an normal object is selected:
              \param object The selected object.
              \param parent_observer The context currently displayed in the table view.

              <b>2) Tree View Mode: 1 object selected:</b>

              If an observer is double clicked with the ObserverHints::SelectionUseSelectedContext hint the following parameters are used:
              \param object 0.
              \param parent_observer The selected observer.

              If an observer is double clicked with no hints or the ObserverHints::SelectionUseParentContext hint, or if an normal object is selected:
              \param object The selected object.
              \param parent_observer The parent context of the selected object. If it does not have a parent, 0.

              \note In Tree View Mode, categories are handled as normal QObjects with the category name accessible through the objectName() function.
              */
            void doubleClickRequest(QObject* object, Observer* parent_observer = 0);
            //! Signal which is emitted when the user pushes up/down in a new observer widget. The new widget is passed as a paramater.
            void newObserverWidgetCreated(ObserverWidget* new_widget);

        private:
            //! Disconnects the clipboard's copy and cut actions from this widget.
            void disconnectClipboard();

        protected:
            void changeEvent(QEvent *e);

            Ui::ObserverWidget *ui;
            ObserverWidgetData* d;
        };
    }
}

#endif // OBSERVERWIDGET_H

