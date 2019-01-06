<template>
  <div>
    <el-table
      :data="templates"
      stripe
      :height="tableHeight">
      <el-table-column
        prop="template.id"
        label="ID"
        width="180">
      </el-table-column>
      <el-table-column
        prop="template.name"
        label="名称"
        width="180">
      </el-table-column>
      <el-table-column
        prop="template.url"
        label="URL">
      </el-table-column>
      <el-table-column
        label="状态">
        <template slot-scope="scope">
          {{ scope.row.template.available | formatStatus }}
        </template>
      </el-table-column>      
      <el-table-column
        prop="template.addTime"
        label="创建时间">
      </el-table-column>
      <el-table-column
        fixed="right"
        label="操作">
        <template slot-scope="scope">
          <el-button type="text" size="small" @click="$router.push({ name: 'Templates.Detail', params: { id: scope.row.template.id } })">详情</el-button>
        </template>
      </el-table-column>
    </el-table>

    <el-pagination
      class="pagination"
      layout="prev, pager, next"
      ref="pagination"
      :page-count="pageCount"
      :current-page="currentPage"
      @current-change="handlePageChange">
    </el-pagination>    
  </div>
</template>

<script>
import Vue from 'vue'
import { mapActions } from 'vuex'
import { Loading } from 'element-ui'

export default {
  data () {
    return {
      tableHeight: 0,

      templates: [],
      currentPage: 1,
      pageCount: 0,
    }
  },

  methods: {
    ...mapActions(['getTemplates']),

    init () {
      this.currentPage = parseInt(this.$route.query.page || 1)
      this.loadData(this.currentPage)
    },

    loadData (page) {
      let params = {
        page
      }
      let loading = Loading.service()
      this.getTemplates(params).then(resp => {
        const data = resp.data
        this.templates = data.templates
        this.pageCount = data.pageCount
        this.currentPage = data.currentPage        
      }).finally(() => loading.close())
    },

    handlePageChange (page) {
      this.$router.push({ name: 'Templates.Index', query: { page } })
    }
  },

  mounted () {
    Vue.nextTick(() => {
      this.tableHeight = window.innerHeight - this.$refs.pagination.$el.clientHeight
    })
  },

  beforeRouteEnter (to, from, next) {
    next(vm => {
      vm.init()
    })
  },

  beforeRouteUpdate (to, from, next) {
    next()
    this.init()
  },

  filters: {
    formatStatus (status) {
      switch (status) {
        case 1:
          return '运行中'
        case 2:
          return '待审'
        default:
          return '停止'
      }
    }
  }
}
</script>

<style type="text/css">
  .pagination {
    padding: 50px 0;
    text-align: center;
  }
</style>
