/*
Dwarf Therapist
Copyright (c) 2009 Trey Stout (chmod)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "gridviewdock.h"
#include "ui_gridviewdock.h"
#include "viewmanager.h"
#include "gridview.h"
#include "gridviewdialog.h"

GridViewDock::GridViewDock(QWidget *parent, Qt::WindowFlags flags)
	: QDockWidget(parent, flags)
	, m_manager(0)
	, ui(new Ui::GridViewDock)
	, m_tmp_item(0)
{}

void GridViewDock::set_view_manager(ViewManager *mgr) {
	ui->setupUi(this);
	m_manager = mgr;
	draw_views();

	connect(ui->list_views, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(edit_view(QListWidgetItem*)));
}

void GridViewDock::draw_views() {
	ui->list_views->clear();
	foreach(GridView *v, m_manager->views()) {
		new QListWidgetItem(v->name(), ui->list_views);
	}
}

void GridViewDock::contextMenuEvent(QContextMenuEvent *e) {
	m_tmp_item = ui->list_views->itemAt(ui->list_views->viewport()->mapFromGlobal(e->globalPos()));
	if (!m_tmp_item)
		return;
	QMenu m(this);
	m.addAction(tr("Edit..."), this, SLOT(edit_view()));
	m.addAction(tr("Delete..."), this, SLOT(delete_view()));
	m.exec(e->globalPos());
}

void GridViewDock::edit_view(QListWidgetItem *item) {
	m_tmp_item = item;
	edit_view();
}

void GridViewDock::edit_view() {
	if (!m_tmp_item)
		return;
	
	GridView *view = m_manager->get_view(m_tmp_item->text());
	if (!view)
		return;
	GridViewDialog *d = new GridViewDialog(m_manager, view, this);
	int accepted = d->exec();
	if (accepted == QDialog::Accepted) {
		view->set_name(d->name());
		view->clear();
		foreach(QString set_name, d->sets()) {
			ViewColumnSet *s = m_manager->get_set(set_name);
			if (s)
				view->add_set(s);
		}
	} else {
		//view->reset_from_disk();
	}
	m_tmp_item = 0;
}
void GridViewDock::delete_view() {
	if (m_tmp_item)
		QMessageBox::information(this, tr("delete"), m_tmp_item->text());
	m_tmp_item = 0;
}
