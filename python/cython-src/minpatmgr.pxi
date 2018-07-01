
### @file minpatmgr.pxi
### @brief minpatmgr の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport vector
from CXX_MinPatMgr cimport MinPatMgr as CXX_MinPatMgr
from CXX_TpgFault cimport TpgFault as CXX_TpgFault
from CXX_TestVector cimport TestVector as CXX_TestVector
from CXX_FaultType cimport FaultType as CXX_FaultType

cdef class MinPatMgr :
    cdef CXX_MinPatMgr _this

    ### @brief 初期化
    ### @param[in] fault_list 故障のリスト
    ### @param[in] tv_list テストパタンのリスト
    ### @param[in] network 対象のネットワーク
    ### @param[in] fault_type 故障の種類
    def __init__(MinPatMgr self, fault_list, tv_list, TpgNetwork network, fault_type) :
        cdef vector[const CXX_TpgFault*] c_fault_list
        cdef vector[CXX_TestVector] c_tv_list
        cdef int nf = len(fault_list)
        cdef int nv = len(tv_list)
        cdef CXX_FaultType c_fault_type = from_FaultType(fault_type)
        cdef TpgFault fault
        cdef TestVector tv
        c_fault_list.resize(nf)
        for i in range(nf) :
            fault = fault_list[i]
            c_fault_list[i] = fault._thisptr
        c_tv_list.resize(nv)
        for i in range(nv) :
            tv = tv_list[i]
            c_tv_list[i] = tv._this
        self._this.init(c_fault_list, c_tv_list, network._this, c_fault_type)

def gen_compat_graph(tv_list) :
    cdef int id1, id2
    cdef TestVector tv1, tv2
    cdef int n = len(tv_list)
    cdef UdGraph graph = UdGraph(n)
    for id1 in range(n - 1) :
        tv1 = tv_list[id1]
        for id2 in range(id1 + 1, n) :
            tv2 = tv_list[id2]
            if not TestVector.is_compatible(tv1, tv2) :
                graph.connect(id1, id2)
    return graph

def gen_colcov(fault_list, tv_list, network, fault_type) :
    cdef int id1, id2
    cdef TestVector tv1, tv2
    cdef int nv = len(tv_list)
    cdef int nf = len(fault_list)
    cdef Fsim fsim = Fsim('Fsim3', network, fault_type)
    cdef TestVector tv
    cdef int wpos = 0
    cdef int tid_base = 0
    cdef int fid = 0
    cdef ColCov colcov = ColCov(nf, nv)
    cdef int vect_size
    cdef int tid
    fid_dict = {}
    for fault in fault_list :
        fid_dict[fault.id] = fid
        fid += 1
    tv_buff = []
    elem_list = []
    for tv in tv_list :
        tv_buff.append(tv)
        if len(tv_buff) == 64 :
            fp_list = fsim.ppsfp(tv_buff)
            for fault, pat_id_list in fp_list :
                fid = fid_dict[fault.id]
                for pat_id in pat_id_list :
                    colcov.insert_elem(fid, tid_base + pat_id)
            tv_buff = []
            tid_base += 64
    if len(tv_buff) > 0 :
        fp_list = fsim.ppsfp(tv_buff)
        for fault, pat_id_list in fp_list :
            fid = fid_dict[fault.id]
            for pat_id in pat_id_list :
                colcov.insert_elem(fid, tid_base + pat_id)

    tv = tv_list[0]
    vect_size = tv.vector_size
    conflict_pair_list = []
    for bit in range(vect_size) :
        list0 = []
        list1 = []
        for tid in range(len(tv_list)) :
            tv = tv_list[tid]
            v = tv.val(bit)
            if v == Val3._0 :
                list0.append(tid)
            elif v == Val3._1 :
                list1.append(tid)
        if len(list0) > 0 and len(list1) > 0 :
            print('{} x {}'.format(len(list0), len(list1)))
            conflict_pair_list.append( (list0, list1) )

    return colcov

def gen_conflict_list(tv_list) :
    cdef int id1, id2
    cdef TestVector tv1, tv2
    cdef int n = len(tv_list)
    #conflict_list = []
    for id1 in range(n - 1) :
        tv1 = tv_list[id1]
        for id2 in range(id1 + 1, n) :
            tv2 = tv_list[id2]
            if not TestVector.is_compatible(tv1, tv2) :
                #conflict_list.append( (id1, id2) )
                yield id1, id2
    #return conflict_list

def gen_elem_list(fault_list, tv_list, network, fault_type) :
    cdef int nf = len(fault_list)
    cdef Fsim fsim = Fsim('Fsim3', network, fault_type)
    cdef TestVector tv
    cdef int wpos = 0
    cdef int tid_base = 0
    cdef int fid = 0
    fid_dict = {}
    for fault in fault_list :
        fid_dict[fault.id] = fid
        fid += 1
    tv_buff = []
    elem_list = []
    for tv in tv_list :
        tv_buff.append(tv)
        if len(tv_buff) == 64 :
            fp_list = fsim.ppsfp(tv_buff)
            for fault, pat_id_list in fp_list :
                fid = fid_dict[fault.id]
                for pat_id in pat_id_list :
                    elem_list.append( (fid, tid_base + pat_id) )
            tv_buff = []
            tid_base += 64
    if len(tv_buff) > 0 :
        fp_list = fsim.ppsfp(tv_buff)
        for fault, pat_id_list in fp_list :
            fid = fid_dict[fault.id]
            for pat_id in pat_id_list :
                elem_list.append( (fid, tid_base + pat_id) )
    return elem_list

"""
def gen_covering_matrix(tv_list, network, fault_type) :
    fsim = Fsim('Fsim3', network, fault_type)
    tv_buff = [ TestVector() for i in range(64) ]
    wpos = 0
    pat_list_dict = dict()
    for tv in tv_list :
        tv_buff[wpos] = tv
        wpos += 1
        if wpos == 64 :
            fp_list = fsim.ppsfp(tv_buff)
            for fault, pat in fp_list :
                for i in range(64) :
                    if pat & (1 << i) :
                        if not fault.id in pat_list_dict :
                            pat_list_dict[fault.id] = list()
                        pat_list_dict[fault.id].append(tv_buff[i])
            wpos = 0
    if wpos > 0 :
        fp_list = fsim.ppsfp(tv_buff)
        for ault, pat in fp_list :
            for i in range(wpos) :
                if pat & (1 << i) :
                    if not fault.id in pat_list_dict :
                        pat_list_dict[fault.id] = list()
                    pat_list_dict[fault.id].append(tv_buff[i])
"""
