/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 */

#include "bcast.h"
#include "planc_ucx_plan.h"
#include "planc_ucx_p2p.h"
#include "core/ucg_dt.h"
#include "core/ucg_group.h"
#include "util/ucg_log.h"
#include "util/ucg_malloc.h"
#include <netdb.h>
#include <arpa/inet.h> /* inet_addr */


static int server_port = 13300;

static ucg_status_t ucg_planc_ucx_bcast_multicast_op_progress(ucg_plan_op_t *ucg_op)
{
    ucg_planc_ucx_op_t *op = ucg_derived_of(ucg_op, ucg_planc_ucx_op_t);
    if (UCS_OK == ucp_tag_send_bcast_progress(op->bcast.mcast_ctx.ucp_mcast_ctx)) {
        ucg_op->super.status = UCG_OK;
        return UCG_OK;
    }
    ucg_op->super.status = UCG_INPROGRESS;
    return UCG_INPROGRESS;
}

static ucg_status_t ucg_planc_ucx_bcast_multicast_op_trigger(ucg_plan_op_t *ucg_op)
{
    ucg_status_t status;
    ucg_planc_ucx_op_t *op = ucg_derived_of(ucg_op, ucg_planc_ucx_op_t);
    ucg_planc_ucx_op_reset(op);
    ucg_coll_bcast_args_t *args = &ucg_op->super.args.bcast;
    ucg_rank_t myrank = op->super.vgroup->myrank;
    uint32_t group_size = op->super.vgroup->size;
    int is_root = !!(args->root == myrank);
    if (!op->bcast.mcast_ctx.init_done) {
        struct sockaddr_in sock_addr   = {
                .sin_family            = AF_INET,
                .sin_port              = htons(server_port),
                .sin_addr              = {
                        .s_addr        = is_root ? INADDR_ANY : inet_addr(op->bcast.mcast_ctx.server_ip)
                }
        };
        ucs_sock_addr_t server_address = {
                .addr                  = (struct sockaddr *)&sock_addr,
                .addrlen               = sizeof(struct sockaddr)
        };

        ucg_info("ucp_bcast_init: myrank=%d is_root=%d group_size=%d", myrank, is_root, group_size);

        status = ucp_tag_send_bcast_init(&op->bcast.mcast_ctx.ucp_mcast_ctx, &server_address, group_size - 1, is_root ? UCP_COLL_BCAST_FLAG_SERVER : 0, myrank, 1000);
        if (status) {
            ucg_info("ucp_bcast_init ERROR: myrank=%d is_root=%d group_size=%d", myrank, is_root, group_size);
            return UCG_ERR_IO_ERROR;
        }
        op->bcast.mcast_ctx.init_done = 1;
        server_port++;
    }
    return ucp_tag_send_bcast_start(op->bcast.mcast_ctx.ucp_mcast_ctx,  args->buffer, args->count, is_root ? UCP_COLL_BCAST_FLAG_SERVER: 0);
}

static ucg_status_t ucg_planc_ucx_bcast_multicast_op_discard(ucg_plan_op_t *ucg_op)
{
    ucg_planc_ucx_op_t *op = ucg_derived_of(ucg_op, ucg_planc_ucx_op_t);
    ucp_tag_send_bcast_destroy(op->bcast.mcast_ctx.ucp_mcast_ctx);
    UCG_CLASS_DESTRUCT(ucg_plan_op_t, ucg_op);
    ucg_mpool_put(ucg_op);
    return UCG_OK;
}

ucg_status_t ucg_planc_ucx_bcast_multicast_prepare(ucg_vgroup_t *vgroup,
                                              const ucg_coll_args_t *args,
                                              ucg_plan_op_t **op)
{
    UCG_CHECK_NULL_INVALID(vgroup, args, op);
    ucg_status_t status;
    ucg_planc_ucx_group_t *ucx_group = ucg_derived_of(vgroup, ucg_planc_ucx_group_t);
    ucg_planc_ucx_op_t *ucx_op = ucg_mpool_get(&ucx_group->context->op_mp);
    ucg_planc_ucx_bcast_config_t *bcast_config;
    if (ucx_op == NULL) {
        return UCG_ERR_NO_MEMORY;
    }

    bcast_config = UCG_PLANC_UCX_CONTEXT_BUILTIN_CONFIG_BUNDLE(ucx_group->context, bcast,
                                                       UCG_COLL_TYPE_BCAST);

    status = UCG_CLASS_CONSTRUCT(ucg_plan_op_t, &ucx_op->super, vgroup,
                                 ucg_planc_ucx_bcast_multicast_op_trigger,
                                 ucg_planc_ucx_bcast_multicast_op_progress,
                                 ucg_planc_ucx_bcast_multicast_op_discard,
                                 args);
    if (status != UCG_OK) {
        ucg_error("Failed to initialize super of ucx op");
        goto err_free_op;
    }
    ucx_op->bcast.mcast_ctx.init_done = 0;
    ucx_op->bcast.mcast_ctx.server_ip = bcast_config->mcast_root_ip;
    ucg_planc_ucx_op_init(ucx_op, ucx_group);

    *op = &ucx_op->super;
    return UCG_OK;

err_free_op:
    ucg_mpool_put(ucx_op);
    return status;

}
