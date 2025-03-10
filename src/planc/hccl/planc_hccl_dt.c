/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#include "planc_hccl_dt.h"


HcclDataType ucg_planc_hccl_dt_table[UCG_DT_TYPE_PREDEFINED_LAST] = {
    [UCG_DT_TYPE_INT8] = HCCL_DATA_TYPE_INT8,
    [UCG_DT_TYPE_INT16] = HCCL_DATA_TYPE_INT16,
    [UCG_DT_TYPE_INT32] = HCCL_DATA_TYPE_INT32,
    [UCG_DT_TYPE_INT64] = HCCL_DATA_TYPE_INT64,
    [UCG_DT_TYPE_UINT8] = HCCL_DATA_TYPE_RESERVED,
    [UCG_DT_TYPE_UINT16] = HCCL_DATA_TYPE_RESERVED,
    [UCG_DT_TYPE_UINT32] = HCCL_DATA_TYPE_RESERVED,
    [UCG_DT_TYPE_UINT64] = HCCL_DATA_TYPE_UINT64,
    [UCG_DT_TYPE_FP16] = HCCL_DATA_TYPE_FP16,
    [UCG_DT_TYPE_FP32] = HCCL_DATA_TYPE_FP32,
    [UCG_DT_TYPE_FP64] = HCCL_DATA_TYPE_RESERVED,
};

HcclReduceOp ucg_planc_hccl_op_table[UCG_OP_TYPE_PREDEFINED_LAST] = {
    [UCG_OP_TYPE_MAX] = HCCL_REDUCE_MAX,
    [UCG_OP_TYPE_MIN] = HCCL_REDUCE_MIN,
    [UCG_OP_TYPE_SUM] = HCCL_REDUCE_SUM,
    [UCG_OP_TYPE_PROD] = HCCL_REDUCE_PROD,
};

int ucg_planc_hccl_dt_size_table[HCCL_DATA_TYPE_RESERVED] = {
    [HCCL_DATA_TYPE_INT8] = 1,
    [HCCL_DATA_TYPE_INT16] = 2,
    [HCCL_DATA_TYPE_INT32] = 4,
    [HCCL_DATA_TYPE_FP16] = 2,
    [HCCL_DATA_TYPE_FP32] = 4,
    [HCCL_DATA_TYPE_INT64] = 8,
    [HCCL_DATA_TYPE_UINT64] = 8,
};