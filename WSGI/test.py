from urllib.parse import urlparse, parse_qs, parse_qsl
import re

link2 = '/osoby/2'

n = re.search('^/osoby/(?P<id>[0-9]+)$', link2)

link = 'http://127.0.0.1/osoby/search?imie=nowak&nazwisko=adam'

m = urlparse(link)
params = parse_qs(m.query)
table = 'osoby'

query = 'SELECT ' + ", ".join(params.keys()) + f' FROM {table}' + ' WHERE '
params_table = []
for key,value in params.items():
    buf = ''
    buf += key;buf += '=';buf += '\'';buf += value[0];buf += '\''
    params_table.append(buf)
query += ' AND '.join(params_table)
print(query)