#!/usr/bin/python3
# -*- coding: UTF-8 -*-

plik_bazy = './osoby.sqlite'
localhost = 'http://127.0.0.1'

from urllib.parse import urlparse, parse_qs, parse_qsl
import re, sqlite3

class OsobyApp:
    def __init__(self, environment, start_response):
        self.env = environment
        self.start_response = start_response
        self.status = '200 OK'
        self.headers = [('Content-Type', 'text/html; charset=UTF-8')]
        self.content = b''

    def __iter__(self):
        try:
            self.route()
        except sqlite3.Error as e:
            s = 'SQLite error: ' + str(e)
            self.failure('500 Internal Server Error', s)
        except ValueError as e:
            s = ': ' + str(e)
            self.failure('500 Internal Server Error', s)
        n = len(self.content)
        self.headers.append(('Content-Length', str(n)))
        self.start_response(self.status, self.headers)
        yield self.content

    def failure(self, status, detail=None):
        self.status = status
        s = '<html>\n<head>\n<title>' + status + '</title>\n</head>\n'
        s += '<body>\n<h1>' + status + '</h1>\n'
        if detail is not None:
            s += '<p>' + detail + '</p>\n'
        s += '</body>\n</html>\n'
        self.content = s.encode('UTF-8')

    def route(self):
        if self.env['PATH_INFO'] == '/psy':
            if self.env['QUERY_STRING'] != "":
                link = localhost + self.env['PATH_INFO'] + '?' + self.env['QUERY_STRING']
                params = parse_qs(urlparse(link).query)
                if params is not {}:
                    self.handle_table_psy(params)              
            else:
                self.handle_table_psy()
            return
        
        if self.env['PATH_INFO'] == '/osoby':
            if self.env['QUERY_STRING'] != "":
                link = 'http://127.0.0.1' + self.env['PATH_INFO'] + '?' + self.env['QUERY_STRING']
                params = parse_qs(urlparse(link).query)
                if params is not {}:
                    self.handle_table(params)  
            else:
                self.handle_table()
            return
        
        m = re.search('^/osoby/(?P<id>[0-9]+)$', self.env['PATH_INFO'])
        if m is not None:
            self.handle_item(m.group('id'))
            return
        m = re.search('^/psy/(?P<id>[0-9]+)$', self.env['PATH_INFO'])
        if m is not None:
            self.handle_item_psy(m.group('id'))
            return

        self.failure('404 Not Found')

    def handle_table(self, params = None):
        if self.env['REQUEST_METHOD'] == 'GET':
            if params is None:
                colnames, rows = self.sql_select(None, 'osoby')
            else:
                colnames, rows = self.sql_select(None, 'osoby', params)
            self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'POST':
            colnames, vals = self.read_tsv()
            q = 'INSERT INTO osoby (' + ', '.join(colnames) + ') VALUES ('
            q += ', '.join(['?' for v in vals]) + ')'
            id = self.sql_modify(q, vals)
            colnames, rows = self.sql_select(id, 'osoby')
            self.send_rows(colnames, rows)
        else:
            self.failure('501 Not Implemented')
   
    def handle_table_psy(self, params = None):
        if self.env['REQUEST_METHOD'] == 'GET':
            if params is None:
                colnames, rows = self.sql_select(None, 'psy')
            else:
                colnames, rows = self.sql_select(None, 'psy', params)
            self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'POST':
            colnames, vals = self.read_tsv()
            q = 'INSERT INTO psy (' + ', '.join(colnames) + ') VALUES ('
            q += ', '.join(['?' for v in vals]) + ')'
            id = self.sql_modify(q, vals)
            colnames, rows = self.sql_select(id, 'psy')
            self.send_rows(colnames, rows)
        else:
            self.failure('501 Not Implemented')
   
    def handle_item(self, id):
        if self.env['REQUEST_METHOD'] == 'GET':
            colnames, rows = self.sql_select(id, 'osoby')
            if len(rows) == 0:
                self.failure('404 Not Found')
            else:
                self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'PUT':
            colnames, vals = self.read_tsv()
            q = 'UPDATE osoby SET '
            q += ', '.join([c + ' = ?' for c in colnames])
            q += ' WHERE id = ' + str(id)
            self.sql_modify(q, vals)
            colnames, rows = self.sql_select(id, 'osoby')
            self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'DELETE':
            q = 'DELETE FROM osoby WHERE id = ' + str(id)
            self.sql_modify(q)
        else:
            self.failure('501 Not Implemented')

    def handle_item_psy(self, id):
        if self.env['REQUEST_METHOD'] == 'GET':
            colnames, rows = self.sql_select(id, 'psy')
            if len(rows) == 0:
                self.failure('404 Not Found')
            else:
                self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'PUT':
            colnames, vals = self.read_tsv()
            q = 'UPDATE psy SET '
            q += ', '.join([c + ' = ?' for c in colnames])
            q += ' WHERE id = ' + str(id)
            self.sql_modify(q, vals)
            colnames, rows = self.sql_select(id, 'psy')
            self.send_rows(colnames, rows)
        elif self.env['REQUEST_METHOD'] == 'DELETE':
            q = 'DELETE FROM psy WHERE id = ' + str(id)
            self.sql_modify(q)
        else:
            self.failure('501 Not Implemented')

    def read_tsv(self):
        f = self.env['wsgi.input']
        n = int(self.env['CONTENT_LENGTH'])
        raw_bytes = f.read(n)
        lines = raw_bytes.decode('UTF-8').splitlines()
        colnames = lines[0].split('\t')
        vals = lines[1].split('\t')
        return colnames, vals

    def send_rows(self, colnames, rows):
        s = '\t'.join(colnames) + '\n'
        for row in rows:
            s += '\t'.join([str(val) for val in row]) + '\n'
        self.content = s.encode('UTF-8')
        self.headers = [('Content-Type', 'text/tab-separated-values; charset=UTF-8')]

    def sql_select(self, id=None, table="", params=None):
        conn = sqlite3.connect(plik_bazy)
        conn.execute("PRAGMA foreign_keys = 1")
        crsr = conn.cursor()
        query = f'SELECT * FROM {table}'
        if params is not None:
            params_table = []
            query = f'SELECT * FROM {table} WHERE '
            for key,value in params.items():
                buf = ''
                buf += key;buf += '=';buf += '\'';buf += value[0];buf += '\''
                params_table.append(buf)
            query += ' AND '.join(params_table)
        elif id is not None:
            query += ' WHERE id = ' + str(id)
        crsr.execute(query)
        colnames = [d[0] for d in crsr.description]
        rows = crsr.fetchall()
        crsr.close()
        conn.close()
        return colnames, rows

    def sql_modify(self, query, params=None):
        conn = sqlite3.connect(plik_bazy)
        conn.execute("PRAGMA foreign_keys = 1")
        crsr = conn.cursor()
        if params is None:
            crsr.execute(query)
        else:
            crsr.execute(query, params)
        rowid = crsr.lastrowid   # id wiersza wstawionego przez INSERT
        crsr.close()
        conn.commit()
        conn.close()
        return rowid

if __name__ == '__main__':
    from wsgiref.simple_server import make_server
    port = 8000
    httpd = make_server('', port, OsobyApp)
    print('Listening on port %i, press ^C to stop.' % port)
    httpd.serve_forever()
