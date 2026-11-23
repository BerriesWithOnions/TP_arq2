import pandas as pd, numpy as np, matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib import font_manager as fm
from matplotlib.ticker import FuncFormatter, ScalarFormatter
for f in fm.findSystemFonts():
    if 'texgyretermes' in f.lower(): fm.fontManager.addfont(f)
plt.rcParams.update({'font.family':'TeX Gyre Termes','font.size':9,'pdf.fonttype':42,
  'axes.grid':True,'grid.alpha':.35,'grid.linestyle':':','legend.fontsize':7.5,'axes.titlesize':9,
  'axes.labelsize':8.5,'xtick.labelsize':8,'ytick.labelsize':8,'mathtext.fontset':'stix'})
R=pd.read_pickle('R.pkl')
sty=[dict(c='black',m='o',ls='-'),dict(c='0.35',m='s',ls='--'),dict(c='0.55',m='^',ls=':')]
P=[1,2,4,8]
def setx(ax):
    ax.set_xscale('log',base=2); ax.set_xticks(P); ax.get_xaxis().set_major_formatter(ScalarFormatter())
    ax.set_xlabel('Threads ($p$; 1 = sequencial)')
def serie(app,n,col,modo='P'):
    out=[]
    for p in P:
        r = R[(R.app==app)&(R.n==n)&(R.modo=='S')].iloc[0] if p==1 else R[(R.app==app)&(R.n==n)&(R.modo==modo)&(R.p==p)].iloc[0]
        out.append(r[col])
    return np.array(out)
def tres(app,fname,ents,labels):
    fig,ax=plt.subplots(1,3,figsize=(7.3,2.55))
    for n,lab,s in zip(ents,labels,sty):
        med=serie(app,n,'med'); mn=serie(app,n,'mn'); mx=serie(app,n,'mx')
        ax[0].errorbar(P,med,yerr=[med-mn,mx-med],color=s['c'],marker=s['m'],ls=s['ls'],ms=4,lw=1.2,capsize=2,label=lab)
        ax[1].plot(P,serie(app,n,'S'),color=s['c'],marker=s['m'],ls=s['ls'],ms=4,lw=1.2,label=lab)
        ax[2].plot(P,serie(app,n,'E'),color=s['c'],marker=s['m'],ls=s['ls'],ms=4,lw=1.2,label=lab)
    ax[0].set_yscale('log'); ax[0].set_ylabel('Tempo (ms), mediana e mín–máx'); ax[0].set_title('(a) Tempo de execução')
    ax[1].plot(P,P,color='0.6',lw=.9,ls='-.',label='Ideal ($S=p$)'); ax[1].axhline(1,color='0.75',lw=.8)
    ax[1].set_ylabel('Speedup $S$'); ax[1].set_title('(b) Speedup')
    ax[2].axhline(100,color='0.6',lw=.9,ls='-.'); ax[2].set_ylabel('Eficiência $E$ (%)'); ax[2].set_title('(c) Eficiência')
    for a in ax: setx(a)
    ax[0].legend(loc='upper right'); ax[1].legend(loc='upper left')
    fig.tight_layout(w_pad=1.0); fig.savefig(f'fig/{fname}.pdf'); plt.close(fig)
tres('mult_mat','fig_mult',[256,512,1024],['256×256','512×512','1024×1024'])
tres('imagens','fig_img',[256,512,1024],['256×256','512×512','1024×1024'])

# contadores
c=R[R.app=='contadores']; seq=c[c.modo=='S'].iloc[0]
fig,ax=plt.subplots(1,3,figsize=(7.3,2.55))
P3=[2,4,8]
for modo,s,lab in (('A',sty[0],'A: mutex compartilhado'),('B',sty[1],'B: contadores locais')):
    g=[c[(c.modo==modo)&(c.p==p)].iloc[0] for p in P3]
    med=np.array([x.med for x in g]); mn=np.array([x.mn for x in g]); mx=np.array([x.mx for x in g])
    ax[0].errorbar(P3,med,yerr=[med-mn,mx-med],color=s['c'],marker=s['m'],ls=s['ls'],ms=4,lw=1.2,capsize=2,label=lab)
    ax[1].plot(P3,[x.S for x in g],color=s['c'],marker=s['m'],ls=s['ls'],ms=4,lw=1.2,label=lab)
    ax[2].plot(P3,[x.E for x in g],color=s['c'],marker=s['m'],ls=s['ls'],ms=4,lw=1.2,label=lab)
ax[0].axhline(seq.med,color='0.55',ls=':',lw=1.1,label='Sequencial'); ax[1].axhline(1,color='0.55',ls=':',lw=1.1,label='$S=1$')
ax[1].plot(P3,P3,color='0.6',lw=.9,ls='-.',label='Ideal ($S=p$)'); ax[2].axhline(100,color='0.6',lw=.9,ls='-.')
for a in ax: a.set_yscale('log'); a.set_xscale('log',base=2); a.set_xticks(P3); a.get_xaxis().set_major_formatter(ScalarFormatter()); a.set_xlabel('Threads ($p$)')
ax[0].set_ylabel('Tempo (ms), mediana e mín–máx'); ax[0].set_title('(a) Tempo de execução')
ax[1].set_ylabel('Speedup $S$'); ax[1].set_title('(b) Speedup')
ax[2].set_ylabel('Eficiência $E$ (%)'); ax[2].set_title('(c) Eficiência')
ax[0].legend(loc='center right',fontsize=7); ax[1].legend(loc='center right',fontsize=7)
fig.tight_layout(w_pad=1.0); fig.savefig('fig/fig_cont.pdf'); plt.close(fig)

# comparacao de escalabilidade (maior entrada)
fig,ax=plt.subplots(figsize=(3.6,2.6))
cfg=[('mult_mat',1024,'P','Mult. matrizes 1024',sty[0]),('imagens',1024,'P','Filtro 1024×1024',sty[1]),('contadores',100000000,'B','Contadores B',sty[2])]
for app,n,modo,lab,s in cfg:
    ax.plot(P,serie(app,n,'S',modo),color=s['c'],marker=s['m'],ls=s['ls'],ms=4,lw=1.2,label=lab)
ax.plot(P,P,color='0.6',lw=.9,ls='-.',label='Ideal ($S=p$)'); ax.axhline(1,color='0.75',lw=.8)
setx(ax); ax.set_ylabel('Speedup $S$'); ax.legend(loc='upper left',fontsize=7)
fig.tight_layout(); fig.savefig('fig/fig_comp.pdf'); plt.close(fig)

# ns por MAC (sequencial)
fig,ax=plt.subplots(figsize=(3.4,2.4))
m=R[(R.app=='mult_mat')&(R.modo=='S')]
ax.bar([str(n) for n in m.n],[r.med*1e6/(r.n**3) for _,r in m.iterrows()],color='0.45',width=.55)
for i,(_,r) in enumerate(m.iterrows()): ax.text(i,r.med*1e6/(r.n**3)+.1,f'{r.med*1e6/(r.n**3):.2f}'.replace('.',','),ha='center',fontsize=8)
ax.set_xlabel('Dimensão $n$'); ax.set_ylabel('Tempo por operação (ns)'); ax.grid(axis='x',alpha=0)
fig.tight_layout(); fig.savefig('fig/fig_nsmac.pdf'); plt.close(fig)
print('ok')
