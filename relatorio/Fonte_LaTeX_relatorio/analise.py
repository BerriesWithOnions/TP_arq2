import pandas as pd, numpy as np, matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib import font_manager as fm
for f in fm.findSystemFonts():
    if 'texgyretermes' in f.lower(): fm.fontManager.addfont(f)
plt.rcParams.update({'font.family':'TeX Gyre Termes','font.size':9,'pdf.fonttype':42,
                     'axes.grid':True,'grid.alpha':.3,'grid.linestyle':':','legend.fontsize':8,
                     'axes.titlesize':9,'figure.dpi':150})

d = pd.read_csv('tempos.csv'); d['ms'] = d.tempo_s*1000
rows=[]
for (app,n,modo,p),g in d.groupby(['aplicacao','entrada','modo','threads'],sort=False):
    t=np.sort(g.ms.values)
    rows.append(dict(app=app,n=n,modo=modo,p=p,med=np.median(t),mean=t.mean(),sd=t.std(ddof=1),mn=t.min(),mx=t.max(),runs=list(g.sort_values('repeticao').ms)))
R=pd.DataFrame(rows)
# speedup/eficiencia (base: mediana sequencial da mesma aplicacao/entrada)
def tseq(r): return R[(R.app==r.app)&(R.n==r.n)&(R.modo=='S')].med.iloc[0]
R['tseq']=R.apply(tseq,axis=1); R['S']=R.tseq/R.med; R['E']=R.S/R.p*100
R['cv']=R.sd/R['mean']*100
R.to_pickle('R.pkl')

def f3(x):  # ~3 algarismos significativos, em ms, formato pt-BR
    if x>=1000: s=f'{x:.0f}'
    elif x>=100: s=f'{x:.0f}'
    elif x>=10: s=f'{x:.1f}'
    elif x>=1: s=f'{x:.2f}'
    else: s=f'{x:.3f}'
    return s.replace('.',',')
def fn(x,k): return f'{x:.{k}f}'.replace('.',',')
def fE(e):
    if e>=10: return fn(e,1)
    if e>=1: return fn(e,2)
    return fn(e,3)
def fS(s):
    return fn(s,2) if s>=0.1 else fn(s,4)

# tabelas de resultados
def tabela(app,nome,ents,modos):
    L=[]
    for n in ents:
        for m,p in modos:
            r=R[(R.app==app)&(R.n==n)&(R.modo==m)&(R.p==p)].iloc[0]
            lab = ('Seq.' if m=='S' else (f'{p}' if app!='contadores' else f'{m}, {p}'))
            if m=='S': S_,E_='1,00','100'
            else: S_,E_=fS(r.S),fE(r.E)
            c1,c2 = ((m if m!='S' else 'Seq.'),str(p)) if app=='contadores' else (str(n),lab)
            L.append(f"{c1} & {c2} & {f3(r.med)} & {f3(r.mn)} & {f3(r.mx)} & {f3(r.sd)} & {S_} & {E_} \\\\")
        L.append('\\midrule')
    L=L[:-1]
    open(f'tab/{nome}.tex','w').write('\n'.join(L)+'\n')
tabela('mult_mat','t_mult',[256,512,1024],[('S',1),('P',2),('P',4),('P',8)])
tabela('imagens','t_img',[256,512,1024],[('S',1),('P',2),('P',4),('P',8)])
tabela('contadores','t_cont',[100000000],[('S',1),('A',2),('A',4),('A',8),('B',2),('B',4),('B',8)])
# contadores: separar A e B com midrule
s=open('tab/t_cont.tex').read().split('\n')
s=[x for x in s if x.strip()]
out=[]
for x in s:
    if x.startswith('A & 2') or x.startswith('B & 2'): out.append('\\midrule')
    out.append(x)
open('tab/t_cont.tex','w').write('\n'.join(out)+'\n')

# apendice: tempos individuais
L=[]
names={'mult_mat':'Mult. matrizes','imagens':'Filtro de imagem','contadores':'Contadores'}
for app in ['mult_mat','imagens','contadores']:
    sub=R[R.app==app]
    for n in sub.n.unique():
        for _,r in sub[sub.n==n].iterrows():
            conf=('Seq.' if r.modo=='S' else (f'P, {r.p}' if app!='contadores' else f'{r.modo}, {r.p}'))
            nn = '' if app=='contadores' else str(n)
            L.append(f"{names[app] if (r.n==sub.n.unique()[0] and r.modo=='S') else ''} & {nn if r.modo=='S' else ''} & {conf} & "+' & '.join(f3(x) for x in r.runs)+' \\\\')
        L.append('\\addlinespace[2pt]')
    L.append('\\midrule')
L=L[:-1]
open('tab/t_apend.tex','w').write('\n'.join(L)+'\n')

# derivados
print(R[['app','n','modo','p','med','mean','sd','mn','mx','S','E','cv']].round(4).to_string())
m=R[(R.app=='mult_mat')&(R.modo=='S')]
print('\nns/MAC seq:'); 
for _,r in m.iterrows(): print(r.n, r.med*1e6/(r.n**3))
print('\nns/MAC por thread-core (wall*p/n^3):')
for _,r in R[(R.app=='mult_mat')&(R.modo=='P')].iterrows(): print(r.n,r.p, r.med*1e6/(r.n**3), r.med*1e6*r.p/(r.n**3))
c=R[R.app=='contadores']
print('\nns/incremento (wall/1e8):'); 
for _,r in c.iterrows(): print(r.modo,r.p,r.med*1e6/1e8)
print('\nrazao A/B'); 
for p in (2,4,8):
    a=c[(c.modo=='A')&(c.p==p)].med.iloc[0]; b=c[(c.modo=='B')&(c.p==p)].med.iloc[0]; s=c[c.modo=='S'].med.iloc[0]
    print(p,a/b,a/s)
print('\nrazao tempos seq img:'); i=R[(R.app=='imagens')&(R.modo=='S')]; print(i[['n','med']].assign(r=i.med/i.med.shift(1)))
print(m[['n','med']].assign(r=m.med/m.med.shift(1)))
