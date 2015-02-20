#!/usr/bin/env python

import ROOT
ROOT.gROOT.SetBatch()
ROOT.gROOT.ProcessLine('gErrorIgnoreLevel = kError;')

import os
import time
import varial.tools
import varial.generators as gen
import itertools
# import varial.toolinterface

dirname = 'VertexCategorization'

varial.settings.rootfile_postfixes += ['.pdf']

varial.settings.git_tag = varial.settings.readgittag('/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_4_0_pre5/src/SimTracker/VertexCategorization/GITTAGGER_LOG.txt')

current_tag = varial.settings.git_tag

cuts = ['NoGenSel-NoCuts', 'NoGenSel-AllCuts'
    ]

varial.settings.defaults_Legend['x_pos'] = 0.80
varial.settings.defaults_Legend['label_width'] = 0.36
varial.settings.defaults_Legend['label_height'] = 0.03
# varial.settings.debug_mode = True
varial.settings.box_text_size = 0.03
varial.settings.colors = {
    'TTJets': 632, 
    'WJets': 878,
    'ZJets': 596, 
    'TpTp_M1000': 870, 
    # 'TpJ_TH_M800_NonTlep': 434,
}

def norm_to_first_bin(wrp):
    histo = wrp.histo.Clone()
    firstbin = histo.GetBinContent(1)
    histo.Scale(1. / firstbin)
    info = wrp.all_info()
    info["lumi"] /= firstbin
    return varial.wrappers.HistoWrapper(histo, **info)

def norm_histos_to_first_bin(wrps):
    for wrp in wrps:
        if isinstance(wrp, varial.wrappers.HistoWrapper):
            yield norm_to_first_bin(wrp)
        else:
            yield wrp

def norm_histos_to_integral(wrps):
    for wrp in wrps:
        if isinstance(wrp, varial.wrappers.HistoWrapper):
            yield varial.operations.norm_to_integral(wrp)
        else:
            yield wrp


def label_axes(wrps):
    for w in wrps:
        if 'TH1' in w.type and w.histo.GetXaxis().GetTitle() == '':
            w.histo.GetXaxis().SetTitle(w.histo.GetTitle())
            w.histo.GetYaxis().SetTitle('events')
            w.histo.SetTitle('')
        yield w


def loader_hook(wrps):
    # wrps = norm_cf_plots(wrps)
    wrps = itertools.ifilter(lambda w: w.histo.Integral(), wrps)
    # wrps = gen.imap_conditional(wrps, lambda w: 'TpJ_TH_M800' in w.sample, gen.op.norm_to_lumi)
    wrps = label_axes(wrps)
    return wrps


def plotter_factory(**kws):
    # kws['filter_keyfunc'] = lambda w: 'TH1' in w.type
    kws['hook_loaded_histos'] = loader_hook
    # kws['save_lin_log_scale'] = True
    # kws['save_log_scale'] = True
    # kws['hook_canvas_pre_build'] = canvas_hook
    # kws['hook_canvas_post_build'] = canvas_hook
    return varial.tools.Plotter(**kws)

def create_name(name):
    return name+'v'+varial.settings.git_tag

    

tagger = varial.tools.GitTagger('/nfs/dust/cms/user/nowatsd/sFrameNew/CMSSW_7_2_1_patch4/src/UHH2/VLQToHiggsPairProd/GITTAGGER_LOG.txt')

tagger.run()



p1 = varial.tools.mk_rootfile_plotter(
    name=create_name(dirname),
    # filter_keyfunc=lambda w: not w.name.startswith('cf_'),
    plotter_factory=plotter_factory,
    combine_files=True
)


time.sleep(1)
p1.run()

varial.tools.WebCreator().run()
# os.system('rm -r ~/www/TprimeAnalysis/%s' % create_name(dirname))
# os.system('cp -r %s ~/www/TprimeAnalysis/' % create_name(dirname))
