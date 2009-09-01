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
#ifndef DFINSTANCE_H
#define DFINSTANCE_H

#include <QtGui>

class Dwarf;
class MemoryLayout;

class DFInstance : public QObject {
	Q_OBJECT
public:
	DFInstance(QObject *parent=0);
	virtual ~DFInstance(){}

	// factory ctor
	virtual bool find_running_copy() = 0;

	// accessors
    uint get_memory_correction() {return m_memory_correction;}
    uint get_base_address() {return m_base_addr;}
	bool is_ok(){return m_is_ok;}
	
	// brute force memory scanning methods
    bool looks_like_vector_of_pointers(const uint &addr);

    virtual QVector<uint> enumerate_vector(const uint &addr) = 0;
    virtual char read_char(const uint &addr) = 0;
    virtual short read_short(const uint &addr) = 0;
    virtual ushort read_ushort(const uint &addr) = 0;
    virtual int read_int(const uint &addr) = 0;
    virtual uint read_raw(const uint &addr, const uint &bytes, void *buffer) = 0;
    uint scan_mem(const QByteArray &needle, const uint &start_address, const uint &end_address, bool &ok);
    QVector<uint> scan_mem_find_all(const QByteArray &needle, const uint &start_address, const uint &end_address);
    virtual QString read_string(const uint &addr) = 0;

    QByteArray get_data(const uint &addr, const uint &size);
    QString pprint(const uint &addr, const uint &size);
    QString pprint(const QByteArray &ba, const uint &start_addr=0);
	
	
	// Mapping methods
    uint find_language_vector();
    uint find_translation_vector();
    uint find_creature_vector();
    uint find_dwarf_race_index();
    uint find_stone_vector();

	// Methods for when we know how the data is layed out
	MemoryLayout *memory_layout() {return m_layout;}
	QVector<Dwarf*> load_dwarves();

	// Writing
    virtual uint write_raw(const uint &addr, const uint &bytes, void *buffer) = 0;
    virtual uint write_string(const uint &addr, const QString &str) = 0;
    virtual uint write_int(const uint &addr, const int &val) = 0;
	

	public slots:
		// if a menu cancels our scan, we need to know how to stop
		void cancel_scan() {m_stop_scan = true;}

protected:
	// handy util methods
    virtual uint calculate_checksum() = 0;

	int m_pid;
	uint m_base_addr;
	uint m_memory_size;
    uint m_memory_correction;
	bool m_stop_scan; // flag that gets set to stop scan loops
	bool m_is_ok;
	MemoryLayout *m_layout;

	// these should probably not be constants but I have no idea how to find the values at runtime
	static const int STRING_BUFFER_OFFSET = 4;
	static const int STRING_LENGTH_OFFSET = 20;
	static const int STRING_CAP_OFFSET = 24;

	private slots:
		void heartbeat();

signals:
	// methods for sending progress information to QWidgets
	void scan_total_steps(int steps);
	void scan_progress(int step);
	void scan_message(const QString &message);
	void connection_interrupted();

};

#endif // DFINSTANCE_H
